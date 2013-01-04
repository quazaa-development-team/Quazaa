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

static QString getGitShortVersionHash()
{
  QProcess process;
  bool result;
  process.start("git rev-parse --short HEAD");
  result = process.waitForFinished(1000);

  if (result)
    return process.readAllStandardOutput().trimmed();
  else
      return "unknown";
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

static int writeFile(const QString& fileName, const int major, const int minor, const QString revision, const QString build)
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
    out << "\tstatic const char* REVISION = \"" << revision << "\";\r\n";
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
  const QString revision = getGitShortVersionHash();
  const QString build = getBuildNumber();

  cout << major << '.' << minor << '.' << revision.toLocal8Bit().constData() << ' ' << build.toLocal8Bit().constData() << endl;

  return writeFile(argv[3], major, minor, revision, build);
}
