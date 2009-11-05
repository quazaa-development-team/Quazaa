#ifndef LIBRARYBUILDER_H
#define LIBRARYBUILDER_H

#include <QFileSystemWatcher>
#include <QObject>

class LibraryBuilder : public QObject
{
public:
	LibraryBuilder();
	void loadLibrary();
};

extern LibraryBuilder QuazaaLibrary;

#endif // LIBRARYBUILDER_H
