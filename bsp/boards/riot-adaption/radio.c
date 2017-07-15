

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
} radio_vars_t;

radio_vars_t radio_vars;

netdev_t *netdev;

static void _event_cb(netdev_t *dev, netdev_event_t event);
static void *_event_loop(void *arg);

// admin
void radio_init(void){

    uint16_t dev_type;

#ifdef MODULE_AT86RF2XX
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
#endif
    netdev = (netdev_t *)&at86rf2xx_dev.netdev.netdev;

    if (_pid <= KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, OW_NETDEV_STACKSIZE, OW_NETDEV_PRIO,
                             THREAD_CREATE_STACKTEST, _event_loop, NULL,
                             OW_NETDEV_NAME);
        if (_pid <= 0) {
            DEBUG("OW couldn't create thread\n");
        }
    }

    netdev->driver->init(netdev);
    netdev->event_callback = _event_cb;
    if (netdev->driver->get(netdev, NETOPT_DEVICE_TYPE, &dev_type,
                            sizeof(dev_type)) < 0) {
        DEBUG("OW couldn't get device type\n");
    }

    /* Enable TX with preloading */
    netdev->driver->set(netdev, NETOPT_PRELOADING, (void *)NETOPT_ENABLE, sizeof(netopt_enable_t));
    /* Enable needed IRQs */
    netdev->driver->set(netdev, NETOPT_RX_START_IRQ, (void *)NETOPT_ENABLE, sizeof(netopt_enable_t));

}

void radio_setStartFrameCb(radio_capture_cbt cb) {
   radio_vars.startFrame_cb  = cb;
}

void radio_setEndFrameCb(radio_capture_cbt cb) {
   radio_vars.endFrame_cb    = cb;
}

// reset
void radio_reset(void){
    netdev->driver->init(netdev);
}
// RF admin
void radio_setFrequency(uint8_t frequency){
    netdev->driver->set(netdev, NETOPT_CHANNEL, &frequency, sizeof(frequency));
}
void radio_rfOn(void){
    netdev->driver->set(netdev, NETOPT_STATE, (void *)NETOPT_STATE_IDLE, sizeof(netopt_state_t));
}
void radio_rfOff(void){
    netdev->driver->set(netdev, NETOPT_STATE, (void *)NETOPT_STATE_OFF, sizeof(netopt_state_t));
}
// TX
void radio_loadPacket(uint8_t* packet, uint16_t len){
    DEBUG("OW radio_loadPacket\n");
    /* NETOPT_PRELOADING w as enabled in the init function so this 
    simply loads data to the device buffer */
    struct iovec pkt = {
        .iov_base = (void *)packet,
        .iov_len = (size_t)len,   /* FCS is written by driver */
    };
    if(netdev->driver->send(netdev, &pkt, 1) < 0){
        DEBUG("OW couldn't load pkt\n");
    }
}

// TODO do I need that?
void radio_txEnable(void){
    DEBUG("radio_txEnable\n");
    // TODO: turn on device?
    radio_rfOn();
}
void radio_txNow(void){
    netdev->driver->set(netdev, NETOPT_STATE, (void *)NETOPT_STATE_TX, sizeof(netopt_state_t));
}
// RX
void radio_rxEnable(void){
    DEBUG("radio_rxEnable\n");
    netdev->driver->set(netdev, NETOPT_STATE, (void *)NETOPT_STATE_IDLE, sizeof(netopt_state_t));

}
void radio_rxNow(void) {
   // nothing to do
}
void radio_getReceivedFrame(uint8_t* bufRead,
                            uint8_t* lenRead,
                            uint8_t  maxBufLen,
                            int8_t* rssi,
                            uint8_t* lqi,
                            bool* crc){

    int len = netdev->driver->recv(netdev, bufRead, (size_t)&lenRead, NULL);
    (void)len;
    *crc=true;
}

// interrupt handlers
//kick_scheduler_t    radio_isr(void);

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        assert(_pid != KERNEL_PID_UNDEF);
        msg_t msg;

        msg.type = OW_NETDEV_MSG_TYPE_EVENT;
        msg.content.ptr = dev;

        if (msg_send(&msg, _pid) <= 0) {
            DEBUG("ow_netdev: possibly lost interrupt.\n");
        }
    }
    else {
        // TODO is this a proper location?
        /* capture the time */
        PORT_TIMER_WIDTH capturedTime = sctimer_readCounter();
        switch (event) {
            DEBUG("ow_netdev: event triggered -> %i\n", event);
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
