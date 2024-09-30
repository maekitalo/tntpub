Tntpub
======

Tntpub implements a publish subcribe pattern.  A client can subscribe to a
topic, which is just a string and will receive any messages sent to that topic
by other clients (or itself).  The server is very simple and needs no
configuration since topics exist if there are subscribers to that. Messages
sent to topics, where nobody id subscribed to will just be discarded.

The server as well as client are implemented in a library. It is simple to
implement a own server with additional features like recovery or integrate the
server in own applications. It runs in the _cxxtools_ event loop.

The main features are:
    * only TCP/IP communication
    * send and receive _cxxtools_ serializable objects or just a plain buffer
    * no configuration
    * no distinction between publisher and subscriber - every client may
      subscribe and can send messages to topics

Technical implementation
------------------------

The system uses _cxxtools_ for TCP/IP communcation and when sending objects
_cxxtools_ binary serialization for transport.

The server runs fully non blocking in the _cxxtools_ event loop. The client can
be used in blocking or non blocking mode. When non blocking the client runs in
a _cxxtools_ event loop and processes and sends _cxxtools_ signals.

Cxxtools logging is used. The server initializes logging by reading
`$PROCESSNAME.properties`, `$PROCESSNAME.json`, `$PROCESSNAME.xml`,
`log.properties`, `log.json` or `log.xml`. See cxxtools documenation for
further details.

Examples
--------

In the demo directory there are some examples.

To run the examples compile the code with `./configure` and `make` in the root
directory. Then the processes can be started from the build directory.

Running example - send and receive messages
-------------------------------------------

To see _tntpub_ in action you need 3 terminals. One will run the server, one
a subscriber and one a publisher.

In the first terminal for the server run the `tntpubsubserver` application from
the `server directory.

    cd server
    ./tntpubsubserver

In the second terminal run the subscriber demo application `exampleReader` with
a parameter for a topic name. It will read messages from the server and output
them as formatted json.

    cd demo
    ./exampleReader mytopic

In the third terminal you will run the publisher demo application
`exampleSender`, which will read one or more _json_ messages from standard in
and send it to the topic passed as argument.

The message class can be seen in the file `demo/mymessage.h`. It is a struct
with a string member called `text` and a numeric member called `number`.

    cd demo
    echo '{text:"Hello",number:42}'|./exampleSender mytopic

Now the message can be seen in the reader terminal. You can start multiple
reader in parallel and each will receive the very same message.

Logging
-------

As said cxxtools logging is used. The easiest way to enable logging is to
create a file called `log.properties` in the directory, where the process runs.
The content is just one line:

    rootlogger=INFO

When the process is started, logging is output to the screen. You may add
some more detailed debug output for specific categories, e.g. for tntpub
by adding a line:

    logger.tntpub=DEBUG

To log to a file use:

    file=tntpub.log

with optionally limit the file size and do rolling

    maxfilesize=10M
    maxbackupindex=0

This will rename _tntpub.log_ to _tntpub.log.0_ when the 10 megabyte limit is
exceeded.
