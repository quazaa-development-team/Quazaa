/*
** $Id$
**
** Copyright © Quazaa Development Team, 2009-2013.
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

#include "quazaaglobals.h"
#include "version.h"

#include "debug_new.h"

QString CQuazaaGlobals::APPLICATION_NAME()
{
    return "Quazaa";
}

#ifdef QT_DEBUG
QString CQuazaaGlobals::APPLICATION_VERSION()
{
    return QString().sprintf("%d.%d Git:%s (debug build)", Version::MAJOR, Version::MINOR, Version::REVISION);
}
#else
QString CQuazaaGlobals::APPLICATION_VERSION()
{
    return QString().sprintf("%d.%d Git:%s", Version::MAJOR, Version::MINOR, Version::REVISION);
}
#endif

QString CQuazaaGlobals::APPLICATION_VERSION_STRING()
{
    return QString().sprintf("%d.%d Git:%s (%s)", Version::MAJOR, Version::MINOR, Version::REVISION, Version::BUILD_DATE);
}

QString CQuazaaGlobals::APPLICATION_FINGERPRINT()
{
    return "QZ" + APPLICATION_VERSION();
}

QString CQuazaaGlobals::APPLICATION_ORGANIZATION_NAME()
{
    return "Quazaa Development Team";
}

QString CQuazaaGlobals::APPLICATION_ORGANIZATION_DOMAIN()
{
    return "http://quazaa.sourceforge.net";
}

QString CQuazaaGlobals::USER_AGENT_STRING()
{
    return APPLICATION_NAME() + "/" + APPLICATION_VERSION().replace(",", ".");
}

#ifdef QT_DEBUG
QString CQuazaaGlobals::VENDOR_CODE()
{
    return "QAZB";
}
#else
QString CQuazaaGlobals::VENDOR_CODE()
{
    return "QAZA";
}
#endif

QString CQuazaaGlobals::MEDIA_OPEN_FILTER()
{
    return tr("All Media Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv *.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma);;" + tr("All Files ") + "(*.*);;" + tr("Video Files ") + "(*.3g2 *.3gp *.3gp2 *.3gpp *.d2v *.divx *.amr *.amv *.asf *.avi *.bik *.drc *.dsa *.dsm *.dss *.dsv *.evo *.flc *.fli *.flic *.flv *.hdmov *.ifo *.ivf *.m1v *.m2p *.m2t *.m2ts *.m2v *.m4v *.mkv *.mov *.mp2v *.mp4 *.mp4v *.mpe *.mpeg *.mpg *.mpv2 *.mpv4 *.mts *.ogm *.ogv *.pss *.pva *.ram *.ratdvd *.rm *.rmm *.roq *.rp *.rmvb *.rpm *.rt *.smi *.smil *.smk *.swf *.tp *.tpr *.ts *.tta *.vob *.vp6 *.wm *.wmp *.wmv);;" + tr("Audio Files") + " (*.aac *.ac3 *.aif *.aifc *.aiff *.alac *.au *.cda *.dts *.flac *.mid *.midi *.m1a *.m2a *.m4a *.m4b *.mka *.mpa *.mpc *.mp2 *.mp3 *.oga *.ogg *.ra *.rmi *.snd *.wav *.wma)";
}

