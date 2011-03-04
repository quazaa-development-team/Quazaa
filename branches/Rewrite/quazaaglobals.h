//
// quazaaglobals.h
//
// Copyright © Quazaa Development Team, 2009-2010.
// This file is part of QUAZAA (quazaa.sourceforge.net)
//
// Quazaa is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Quazaa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Quazaa; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef QUAZAAGLOBALS_H
#define QUAZAAGLOBALS_H

#include <QObject>

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
		return "0,0,1,8A";
	}
#else
        static QString APPLICATION_VERSION()
	{
		return "0,0,1,8";
	}
#endif
        static QString APPLICATION_VERSION_STRING()
	{
                return "0.0.1.8a (rev.559 2010-09-08)";
	}
        static QString APPLICATION_FINGERPRINT()
	{
		return "QZ,0,0,1,8";
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
