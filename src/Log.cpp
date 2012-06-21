#include "Log.h"

ofstream Log::m_stream;
QString Log::currentLogFileName;

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
Log::Log() {
}


//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
Log::~Log() {}


//---------------------------------------------------------------
// This function writes an error message into the Log file.
//
// 'errlogline' - QString that contains the error message.
//---------------------------------------------------------------
void Log::writeError(QString errlogline)
{
	//open the Log file and write the error message
	if(open_file())
	{
		m_stream << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss").toStdString() << "\t***Error: " << errlogline.toStdString() << "***" << endl;
		close_file();
	}
}


//---------------------------------------------------------------
// This function writes a message into the Log file
//
// 'logline' - QString that contains the message.
//---------------------------------------------------------------
void Log::write(QString logline)
{
	//open the Log file and write the message
	if(open_file())
	{
		m_stream << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss").toStdString() << "\t" << logline.toStdString() << endl;
		close_file();
	}
}


//---------------------------------------------------------------
// This function will open the Log file
//
// return  'true'  - if open file was successful
//         'false' - if there was an error
//---------------------------------------------------------------
bool Log::open_file(void)
{
 	//open file and set the stream's position indicator to the end
	//of the stream before each output operation.
	m_stream.open(currentLogFileName.toAscii().data(), ios::app);

	//check if open the file was sucessful
	if (m_stream.is_open())
	{
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------------------------------------------
// This function will close the Log file
//
// return  'true'  - if closing the file was successful
//         'false' - if there was an error or no file to close
//-------------------------------------------------------------
bool Log::close_file(void)
{
	//check if the file is open. if yes, close the file,
	//otherwise return false
	if (m_stream.is_open())
	{
		m_stream.close();
		return true;
	}
	else
	{
		return false;
	}
}


//---------------------------------------------------------------
// This function initializes the Log file. It will clear the file
// if the file already exists. It will create the file if it
// doesn't exist yet.
//
// return  'true'  - initalizing was successful
//         'false' - if there was an error
//-------------------------------------------------------------
bool Log::logfile_init()
{
    currentLogFileName = "./log/" + QDate::currentDate().toString("yyyy-MM-dd") + ".log";

    //open file and discard any current content, assuming a length
	//of zero on opening.
	m_stream.open(currentLogFileName.toAscii().data(), ios::app);

	//check if the file is open. if yes, close the file,
	//otherwise return false
	if (m_stream.is_open())
	{
		m_stream.close();
		return true;
	}
	else
	{
		return false;
	}
}

