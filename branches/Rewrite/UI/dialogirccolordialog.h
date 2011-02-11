#ifndef DIALOGIRCCOLORDIALOG_H
#define DIALOGIRCCOLORDIALOG_H

#include <QDialog>

namespace Ui {
    class DialogIRCColorDialog;
}

class DialogIRCColorDialog : public QDialog
{
    Q_OBJECT

public:
	explicit DialogIRCColorDialog(QColor initialColor, QWidget *parent = 0);
	~DialogIRCColorDialog();
	QColor m_oIRCColor;

private slots:
	void on_toolButtonWhite_clicked();

	void on_toolButtonBlack_clicked();

	void on_toolButtonNavy_clicked();

	void on_toolButtonGreen_clicked();

	void on_toolButtonGray_clicked();

	void on_toolButtonLightGray_clicked();

	void on_toolButtonBlue_clicked();

	void on_toolButtonDarkCyan_clicked();

	void on_toolButtonCyan_clicked();

	void on_toolButtonPurple_clicked();

	void on_toolButtonMagenta_clicked();

	void on_toolButtonLime_clicked();

	void on_toolButtonMaroon_clicked();

	void on_toolButtonRed_clicked();

	void on_toolButtonOrange_clicked();

	void on_toolButtonYellow_clicked();

	void on_toolButton_2_clicked();

private:
    Ui::DialogIRCColorDialog *ui;
};

#endif // DIALOGIRCCOLORDIALOG_H
