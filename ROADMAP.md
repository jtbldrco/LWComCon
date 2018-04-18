# LWComCon Road Map

This draft Road Map summarizes candidates for next development ...

- Connection Resiliency: project lwcomcon_full shows that sometimes (?) when a node goes
down and is brought back up, reconnections do not take place as expected/desired.
- Further encapsulation of MsgCommHdlr: Class MsgCommHdlr can be updated to encapsulate
further the recurring logic of connection setup and monitoring.  The idea will be to
push this boilerplate logic into the encapsulation (and rename the class
MessageCommHandler), then have that class accept Functors which handle the actions
associated with pushing and popping messages to/from the internal ThreadSafePtrQueue.
- Scale and throughput: Class DivisibleConsumer shows that it lags the output of
DivisibleProducer.  Make DivisibleConsumer a manager of multiple consumer threads
(based on class ThreadedWorker); have DivisibleConsumer accept ComCon messages to
respond to requests to add (or teardown) consumer queues.
- Move all mainLoop work to ThreadedWorker objects: Class DivisibleConsumer does
its actual 'consumption' on the main thread.  With above item (multiple consumer
threads) these will be moved to ThreadedWorker objects and the main thread will
monitor for and respond to Com/Con messages only (add/teardown consumers).
- Add Control over Production rate: Class DivisibleProducer can produce work at
various rates - add Com/Con message to increase/decrease that rate.
- Class LWComConFull: add menu items to control DivisibleProducer and DivisibleConsumer as
mentioned above; further, extend it to monitor queue lengths and automate the response.
