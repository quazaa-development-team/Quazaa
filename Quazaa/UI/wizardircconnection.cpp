#include "wizardircconnection.h"
#include "ui_wizardircconnection.h"

#include "quazaasettings.h"

WizardIrcConnection::WizardIrcConnection(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardIrcConnection)
{
	ui->setupUi(this);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/Resource/Chat/Chat.png"));
	setWindowFilePath(tr("Connection"));

	quazaaSettings.loadChatConnectionWizard();
	//Users page
    lineEditNickName = new CompleterLineEdit(quazaaSettings.Chat.NickNames, this);
    QRegExpValidator* validator = new QRegExpValidator(lineEditNickName);
	validator->setRegExp(QRegExp("\\S+"));
    lineEditNickName->setValidator(validator);
    ui->horizontalLayoutNick->addWidget(lineEditNickName);
    connect(lineEditNickName, SIGNAL(textChanged(QString)), this, SLOT(completeChanged()));
    ui->labelNick->setBuddy(lineEditNickName);

    lineEditRealName = new CompleterLineEdit(quazaaSettings.Chat.RealNames, this);
    ui->horizontalLayoutReal->addWidget(lineEditRealName);
    ui->labelReal->setBuddy(lineEditRealName);

	// Server page
	connect(ui->spinBoxPort, SIGNAL(valueChanged(int)), this, SLOT(completeChanged()));

    lineEditHost = new CompleterLineEdit(quazaaSettings.Chat.Hosts, this);
    ui->horizontalLayoutHost->addWidget(lineEditHost);
    connect(lineEditHost, SIGNAL(textChanged(QString)), this, SLOT(completeChanged()));
    ui->labelHost->setBuddy(lineEditHost);

    lineEditUserName = new CompleterLineEdit(quazaaSettings.Chat.UserNames, this);
    ui->horizontalLayoutUserName->addWidget(lineEditUserName);
    connect(lineEditUserName, SIGNAL(textChanged(QString)), this, SLOT(completeChanged()));
    ui->labelUser->setBuddy(lineEditUserName);

	// Connection page
	lineEditConnectionName = new CompleterLineEdit(quazaaSettings.Chat.ConnectionNames, this);
	ui->horizontalLayoutConnectionName->addWidget(lineEditConnectionName);
	ui->labelConnectionName->setBuddy(lineEditConnectionName);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(completeChanged()));
    button(QWizard::NextButton)->setEnabled(false);
}

WizardIrcConnection::~WizardIrcConnection()
{
	delete ui;
}

void WizardIrcConnection::accept()
{
	if (!quazaaSettings.Chat.NickNames.contains(nickName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.NickNames << nickName();
	if (!quazaaSettings.Chat.RealNames.contains(realName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.RealNames << realName();
	if (!quazaaSettings.Chat.Hosts.contains(hostName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.Hosts << hostName();
    if (!quazaaSettings.Chat.UserNames.contains(userName(), Qt::CaseInsensitive))
        quazaaSettings.Chat.UserNames << userName();
	if (!quazaaSettings.Chat.ConnectionNames.contains(connectionName(), Qt::CaseInsensitive))
		quazaaSettings.Chat.ConnectionNames << connectionName();
	quazaaSettings.saveChatConnectionWizard();

	QDialog::accept();
}

ConnectionInfo WizardIrcConnection::connection() const
{
	ConnectionInfo connection;
	connection.nick = nickName();
	connection.real = realName();
	connection.host = hostName();
	connection.port = port();
	connection.secure = isSecure();
    connection.user = userName();
	connection.pass = password();
	connection.name = connectionName();
	return connection;
}

void WizardIrcConnection::setConnection(const ConnectionInfo& connection)
{
	setNickName(connection.nick);
	setRealName(connection.real);
	setHostName(connection.host);
	setPort(connection.port);
	setSecure(connection.secure);
    setUserName(connection.user);
	setPassword(connection.pass);
	setConnectionName(connection.name);
}

QString WizardIrcConnection::nickName() const
{
    return lineEditNickName->text();
}

void WizardIrcConnection::setNickName(const QString& nickName)
{
    lineEditNickName->setText(nickName);
}

QString WizardIrcConnection::realName() const
{
    return lineEditRealName->text();
}

void WizardIrcConnection::setRealName(const QString& realName)
{
    lineEditRealName->setText(realName);
}

QString WizardIrcConnection::hostName() const
{
    return lineEditHost->text();
}

void WizardIrcConnection::setHostName(const QString& hostName)
{
	lineEditHost->setText(hostName);
}

quint16 WizardIrcConnection::port() const
{
	return ui->spinBoxPort->value();
}

void WizardIrcConnection::setPort(quint16 port)
{
	ui->spinBoxPort->setValue(port);
}

bool WizardIrcConnection::isSecure() const
{
	return ui->checkBoxSecure->isChecked();
}

void WizardIrcConnection::setSecure(bool secure)
{
    ui->checkBoxSecure->setChecked(secure);
}

QString WizardIrcConnection::userName() const
{
    return lineEditUserName->text();
}

void WizardIrcConnection::setUserName(const QString &userName)
{
    lineEditUserName->setText(userName);
}

QString WizardIrcConnection::password() const
{
	return ui->lineEditPassword->text();
}

void WizardIrcConnection::setPassword(const QString& password)
{
	ui->lineEditPassword->setText(password);
}

QString WizardIrcConnection::connectionName() const
{
	return lineEditConnectionName->text();
}

void WizardIrcConnection::setConnectionName(const QString& name)
{
	lineEditConnectionName->setText(name);
}

bool WizardIrcConnection::isComplete() const
{
	if (currentPage() == ui->wizardPageUser)
        return !lineEditNickName->text().isEmpty();
	else if (currentPage() == ui->wizardPageServer)
		return !lineEditHost->text().isEmpty() && ui->spinBoxPort->hasAcceptableInput();
	else
		return true;
}

void WizardIrcConnection::completeChanged()
{
    button(QWizard::NextButton)->setEnabled(isComplete());
}
