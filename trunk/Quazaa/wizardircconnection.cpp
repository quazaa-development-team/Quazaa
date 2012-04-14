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
	lineEditNick = new CompleterLineEdit(quazaaSettings.Chat.NickNames, this);
	QRegExpValidator* validator = new QRegExpValidator(lineEditNick);
	validator->setRegExp(QRegExp("\\S+"));
	lineEditNick->setValidator(validator);
	ui->horizontalLayoutNick->addWidget(lineEditNick);
	connect(lineEditNick, SIGNAL(textChanged(QString)), this, SLOT(completeChanged()));
	ui->labelNick->setBuddy(lineEditNick);

	lineEditReal = new CompleterLineEdit(quazaaSettings.Chat.RealNames, this);
	ui->horizontalLayoutReal->addWidget(lineEditReal);
	ui->labelReal->setBuddy(lineEditReal);

	// Server page
	connect(ui->spinBoxPort, SIGNAL(valueChanged(int)), this, SLOT(completeChanged()));

	lineEditHost = new CompleterLineEdit(quazaaSettings.Chat.Hosts, this);
	ui->horizontalLayoutHost->addWidget(lineEditHost);
	connect(lineEditHost, SIGNAL(textChanged(QString)), this, SLOT(completeChanged()));
	ui->labelHost->setBuddy(lineEditHost);

	// Connection page
	lineEditConnectionName = new CompleterLineEdit(quazaaSettings.Chat.ConnectionNames, this);
	ui->horizontalLayoutConnectionName->addWidget(lineEditConnectionName);
	ui->labelConnectionName->setBuddy(lineEditConnectionName);
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
	setPassword(connection.pass);
	setConnectionName(connection.name);
}

QString WizardIrcConnection::nickName() const
{
	return lineEditNick->text();
}

void WizardIrcConnection::setNickName(const QString& nickName)
{
	lineEditNick->setText(nickName);
}

QString WizardIrcConnection::realName() const
{
	return lineEditReal->text();
}

void WizardIrcConnection::setRealName(const QString& realName)
{
	lineEditReal->setText(realName);
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
		return !lineEditNick->text().isEmpty();
	else if (currentPage() == ui->wizardPageServer)
		return !lineEditHost->text().isEmpty() && ui->spinBoxPort->hasAcceptableInput();
	else
		return true;
}

void WizardIrcConnection::completeChanged()
{
	this->button(QWizard::NextButton)->setEnabled(isComplete());
}
