

#include "radio.h"
#include "sctimer.h"

#include "net/netdev.h"
#include "net/netopt.h"


#define ENABLE_DEBUG                (1)
#include "debug.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif


#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif


#define OW_NETDEV_NAME            "ow_netdev"
#define OW_NETDEV_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define OW_NETDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#define OW_NETDEV_QUEUE_LEN       (8)
#define OW_NETDEV_MSG_TYPE_EVENT   0x666 // TODO ADAPT!!!

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _stack[OW_NETDEV_STACKSIZE];
static msg_t _queue[OW_NETDEV_QUEUE_LEN];

//=========================== variables =======================================

typedef struct {
    radio_capture_cbt         startFrame_cb;
    radio_capture_cbt         endFrame_cb;
    radio_state_t             state;
    netdev_t                 *dev;
} radio_vars_t;

radio_vars_t radio_vars;

static void _event_cb(netdev_t *dev, netdev_event_t event);
static void *_event_loop(void *arg);

// admin
void radio_init(void) {

    uint16_t dev_type;
    // clear variables
    memset(&radio_vars,0,sizeof(radio_vars_t));
    radio_vars.state = RADIOSTATE_STOPPED;

#ifdef MODULE_AT86RF2XX
    radio_vars.dev = (netdev_t *)&at86rf2xx_dev.netdev.netdev;
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
#endif

    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, OW_NETDEV_STACKSIZE, OW_NETDEV_PRIO,
                             THREAD_CREATE_STACKTEST, _event_loop, NULL,
                             OW_NETDEV_NAME);
        if (_pid <= 0) {
            DEBUG("OW couldn't create thread\n");
        }
    }

    radio_vars.dev->driver->init(radio_vars.dev);
    radio_vars.dev->event_callback = _event_cb;
    if (radio_vars.dev->driver->get(radio_vars.dev, NETOPT_DEVICE_TYPE, &dev_type,
                                    sizeof(dev_type)) < 0) {
        DEBUG("OW couldn't get device type\n");
    }

    netopt_enable_t enable;
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_PROMISCUOUSMODE, &(enable), sizeof(netopt_enable_t));
    /* Enable needed IRQs */
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RX_START_IRQ, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RX_END_IRQ, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_DISABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_AUTOACK, &(enable), sizeof(netopt_enable_t));
    /* Enable TX with preloading */
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_PRELOADING, &(enable), sizeof(netopt_enable_t));
    enable = NETOPT_ENABLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RAWMODE, &(enable), sizeof(netopt_enable_t));
    uint8_t retrans = 0;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_RETRANS, &(retrans), sizeof(uint8_t));

    radio_vars.state          = RADIOSTATE_RFOFF;
}

void radio_setStartFrameCb(radio_capture_cbt cb) {
    radio_vars.startFrame_cb  = cb;
}

void radio_setEndFrameCb(radio_capture_cbt cb) {
    radio_vars.endFrame_cb    = cb;
}

// reset
void radio_reset(void) {
    netopt_state_t state = NETOPT_STATE_RESET;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
}
// RF admin
void radio_setFrequency(uint8_t frequency) {
    // change state
    radio_vars.state = RADIOSTATE_SETTING_FREQUENCY;

    // configure the radio to the right frequency
    // channels are 16bit in RITOS
    uint16_t chan = frequency;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_CHANNEL, &(chan), sizeof(uint8_t));

    // change state
    radio_vars.state = RADIOSTATE_FREQUENCY_SET;
}

void radio_rfOn(void) {
    netopt_state_t state = NETOPT_STATE_IDLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
}

void radio_rfOff(void) {
    netopt_state_t state = NETOPT_STATE_OFF;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));
    leds_radio_off();

    // change state
    radio_vars.state = RADIOSTATE_RFOFF;
}
// TX
void radio_loadPacket(uint8_t* packet, uint16_t len) {
    DEBUG("OW radio_loadPacket\n");
    /* NETOPT_PRELOADING w as enabled in the init function so this
       simply loads data to the device buffer */
    struct iovec pkt = {
        .iov_base = (void *)packet,
        .iov_len = (size_t)len,   /* FCS is written by driver */
    };
    if (radio_vars.dev->driver->send(radio_vars.dev, &pkt, 1) < 0) {
        DEBUG("OW couldn't load pkt\n");
    }
    // change state
    radio_vars.state = RADIOSTATE_PACKET_LOADED;
}

void radio_txEnable(void) {
    DEBUG("radio_txEnable\n");

    // change state
    radio_vars.state = RADIOSTATE_ENABLING_TX;

   leds_radio_on();
    // change state
    radio_vars.state = RADIOSTATE_TX_ENABLED;
}
void radio_txNow(void) {
    netopt_state_t state = NETOPT_STATE_TX;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &state, sizeof(netopt_state_t));
}
// RX
void radio_rxEnable(void) {
    DEBUG("radio_rxEnable\n");
    // change state
    radio_vars.state = RADIOSTATE_ENABLING_RX;

   leds_radio_on();

    netopt_state_t state = NETOPT_STATE_IDLE;
    radio_vars.dev->driver->set(radio_vars.dev, NETOPT_STATE, &(state), sizeof(netopt_state_t));

    // change state
    radio_vars.state = RADIOSTATE_LISTENING;
}
void radio_rxNow(void) {
    // nothing to do
}
void radio_getReceivedFrame(uint8_t* bufRead,
                            uint8_t* lenRead,
                            uint8_t  maxBufLen,
                            int8_t* rssi,
                            uint8_t* lqi,
                            bool* crc) {
    netdev_ieee802154_rx_info_t rx_info;
    int bytes_expected = radio_vars.dev->driver->recv(radio_vars.dev, bufRead, maxBufLen, &rx_info);

    if (bytes_expected) {
        netdev_ieee802154_rx_info_t rx_info;
        int nread = radio_vars.dev->driver->recv(radio_vars.dev, bufRead, bytes_expected, &rx_info);
        *crc = true;
        if (nread <= 0) {
            return;
        }
        *lenRead = nread;

        *lqi  = 0;
        *rssi = 0;

        /* TODO: check CRC */
        *crc = true;
    }
}

// interrupt handlers
//kick_scheduler_t    radio_isr(void);

PORT_TIMER_WIDTH capturedTime = 0u;
static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        // TODO is this a proper location?
        /* capture the time */
        capturedTime = sctimer_readCounter();
        assert(_pid != KERNEL_PID_UNDEF);
        msg_t msg;

        msg.type = OW_NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = dev;

        if (msg_send(&msg, _pid) <= 0) {
            DEBUG("ow_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        DEBUG("ow_netdev: event triggered -> %i\n", event);
        assert( capturedTime != 0u);
        switch (event) {
            case NETDEV_EVENT_RX_STARTED:
                radio_vars.startFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_RX_STARTED\n");
                break;
            case NETDEV_EVENT_RX_COMPLETE:
                radio_vars.endFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_RX_COMPLETE\n");
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                radio_vars.endFrame_cb(capturedTime);
                DEBUG("NETDEV_EVENT_TX_COMPLETE\n");
                break;
            default:
                break;
        }
    }
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_init_queue(_queue, OW_NETDEV_QUEUE_LEN);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == OW_NETDEV_MSG_TYPE_EVENT) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
    }
    return NULL;
}
