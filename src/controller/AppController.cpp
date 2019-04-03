#include "AppController.h"



void AppController::loggerInit()
{
	boost::log::add_file_log
	(
		boost::log::keywords::file_name = "errors.log",
		boost::log::keywords::format =
		(
			boost::log::expressions::stream
			<< "[" << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			<< "] [" << boost::log::trivial::severity
			<< "] " << boost::log::expressions::smessage
			)
	);

	boost::log::add_common_attributes();
}