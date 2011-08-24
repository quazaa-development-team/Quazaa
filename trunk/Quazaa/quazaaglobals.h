/*
** quazaaglobals.h
**
** Copyright © Quazaa Development Team, 2009-2011.
** This file is part of QUAZAA (quazaa.sourceforge.net)
**
** Quazaa is free software; this file may be used under the terms of the GNU
** General Public License version 3.0 or later as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Quazaa is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** Please review the following information to ensure the GNU General Public
** License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** You should have received a copy of the GNU General Public License version
** 3.0 along with Quazaa; if not, write to the Free Software Foundation,
** Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef QUAZAAGLOBALS_H
#define QUAZAAGLOBALS_H

#include <QObject>
#include "version.h"

class QuazaaGlobals : public QObject
{
        Q_OBJECT
public:
        static QString APPLICATION_NAME()
	{
		return "Quazaa";
	}
#ifdef QT_DEBUG
        static QString APPLICATION_VERSION()
	{
			return QString().sprintf("%d.%d.r%d (debug build)", Version::MAJOR, Version::MINOR, Version::REVISION);
	}
#else
        static QString APPLICATION_VERSION()
	{
		return QString().sprintf("%d.%d.r%d", Version::MAJOR, Version::MINOR, Version::REVISION);
	}
#endif
        static QString APPLICATION_VERSION_STRING()
	{
		return QString().sprintf("%d.%d.r%d (%s)", Version::MAJOR, Version::MINOR, Version::REVISION, Version::BUILD_DATE);
	}
        static QString APPLICATION_FINGERPRINT()
	{
		return "QZ" + APPLICATION_VERSION();
	}
        static QString APPLICATION_ORGANIZATION_NAME()
	{
		return "Quazaa Development Team";
	}
        static QString APPLICATION_ORGANIZATION_DOMAIN()
	{
		return "http://quazaa.sourceforge.net";
	}
        static QString USER_AGENT_STRING()
	{
                return APPLICATION_NAME() + "/" + APPLICATION_VERSION().replace(",", ".");
	}
#ifdef QT_DEBUG
        static QString VENDOR_CODE()
	{
		return "QAZB";
	}
#else
        static QString VENDOR_CODE()
	{
		return "QAZA";
	}
#endif
        QString MEDIA_OPEN_FILTER()
	{
		return tr("All Media Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv *.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma);;" + tr("All Files ") + "(*.*);;" + tr("Video Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv);;" + tr("Audio Files") + " (*.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma)";
	}
};

#endif // QUAZAAGLOBALS_H
