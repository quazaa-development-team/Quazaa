// Credit goes to Rob Caldecott
// http://qtcreator.blogspot.com/

#include <iostream>
#include <QProcess>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QFileInfo>
#include <QTemporaryFile>
#include <cstdlib>

using namespace std;

static QString getBuildNumber()
{
  const QDateTime today(QDateTime::currentDateTimeUtc());
  return (today.date().toString("yyyy-MM-dd"));
}

static int getSubversionRevision()
{
  int revision = 0;
  QProcess process;
  process.start("svnversion", QStringList() << "." << "--no-newline");
  if (process.waitForStarted() && process.waitForReadyRead())
  {
	const QString str(process.readAll().constData());
	const int pos = str.indexOf(':');
	if (pos != -1)
	{
	  revision = atoi(str.mid(pos + 1).toLocal8Bit().constData());
	}
	else
	{
	  revision = atoi(str.toLocal8Bit().constData());
	}
	process.waitForFinished();
  }
  else
  {
	  cout << "Cannot start svnversion. Make sure it is installed and it is in your PATH.\n";
  }
  return revision;
}

static QByteArray readFile(const QString& fileName)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
  {
	return QByteArray();
  }
  return file.readAll();
}

static int writeFile(const QString& fileName, const int major, const int minor, const int revision, const QString build)
{
  // Create a temp file containing the version info and
  // only replace the existing one if they are different
  QTemporaryFile tempFile;
  if (tempFile.open())
  {
	QTextStream out(&tempFile);
	out << "#ifndef VERSION_H\r\n";
	out << "#define VERSION_H\r\n\r\n";
	out << "namespace Version\r\n";
	out << "{\r\n";
	out << "\tstatic const int MAJOR = " << major << ";\r\n";
	out << "\tstatic const int MINOR = " << minor << ";\r\n";
	out << "\tstatic const int REVISION = " << revision << ";\r\n";
	out << "\tstatic const char* BUILD_DATE = \"" << build << "\";\r\n";
	out << "}\r\n\r\n";
	out << "#endif // VERSION_H\r\n";

	const QString tempFileName = tempFile.fileName();
	tempFile.close();

	if (!QFile::exists(fileName) || readFile(fileName) != readFile(tempFileName))
	{
	  QFile::remove(fileName);
	  QFile::copy(tempFileName, fileName);
	}

	return 0;
  }
  else
  {
	cout << "Error creating temporary file!" << endl;
	return 1;
  }
}

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
	cout << "Usage: version major minor filename" << endl;
	return 1;
  }

  const int major = atoi(argv[1]);
  const int minor = atoi(argv[2]);
  const int revision = getSubversionRevision();
  const QString build = getBuildNumber();

  cout << major << '.' << minor << '.' << revision << ' ' << build.toLocal8Bit().constData() << endl;

  return writeFile(argv[3], major, minor, revision, build);
}
