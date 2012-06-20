#include "SystemCommand.h"
#include <stdio.h>

SystemCommand::SystemCommand() {

}

SystemCommand::~SystemCommand() {

}

QString SystemCommand::getStringFromSystemCommand(QString cmd)
{
  // setup
  QString data;
  FILE *stream;
  char buffer[2048];

  // do it
  stream = popen(cmd.toStdString().c_str(), "r");
  while ( fgets(buffer, 2048, stream) != NULL )
    data.append(buffer);
  pclose(stream);

  // exit
  return data.trimmed();
}
