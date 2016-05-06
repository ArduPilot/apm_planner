## QsLog - the simple Qt logger ##
QsLog is an easy to use logger that is based on Qt's QDebug class. QsLog is released as open source, under the MIT license. 

###Contribution policy###
Bug fixes are welcome, larger changes however are not encouraged at this point due to the lack of time on my side for reviewing and integrating them. Your best bet in this case would be to open a ticket for your change or forking the project and implementing your change there, with the possibility of having it integrated in the future. 
All contributions will be credited, license of the contributions should be MIT. 

### Features ###
* Six logging levels (from trace to fatal)
* Logging level threshold configurable at runtime.
* Minimum overhead when logging is turned off.
* Supports multiple destinations, comes with file and debug destinations.
* Thread-safe
* Supports logging of common Qt types out of the box.
* Small dependency: just drop it in your project directly.

### Usage ###
* Include QsLog.h. Include QsLogDest.h only where you create/add destinations.
* Get the instance of the logger by calling QsLogging::Logger::instance();
* Optionally set the logging level. Info is default.
* Create as many destinations as you want by using the QsLogging::DestinationFactory.
* Add the destinations to the logger instance by calling addDestination.

**Note**: The logger does not take ownership of the destinations. Make sure that the destinations still exist when you call one of the logging macros. A good place to create the destinations is the program's main function.

### Disabling logging ###
Sometimes it's necessary to turn off logging. This can be done in several ways:

* globally, at compile time, by enabling the QS_LOG_DISABLE macro in the supplied .pri file.
* globally, at run time, by setting the log level to "OffLevel".
* per file, at compile time, by including QsLogDisableForThisFile.h in the target file.

### Thread safety ###
The Qt docs say: 
A **thread-safe** function can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized.
A **reentrant** function can also be called simultaneously from multiple threads, but only if each invocation uses its own data.

Since sending the log message to the destinations is protected by a mutex, the logging macros are thread-safe provided that the log has been initialized - i.e: instance() has been called. 
The instance function and the setup functions (e.g: setLoggingLevel, addDestination) are NOT thread-safe and are NOT reentrant.