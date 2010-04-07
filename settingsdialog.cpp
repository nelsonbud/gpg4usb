/*
 *      settingsdialog.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "settingsdialog.h"

#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QLabel>
#include <QButtonGroup>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QTranslator>

class QLabel;
class QButtonGroup;
class QGroupBox;

SettingsDialog::SettingsDialog()
{
    setWindowTitle(tr("Settings"));
    resize(500, 200);
    setModal(true);

	
	/*****************************************
	 * Icon-Size-Box
	 *****************************************/
    iconSizeBox = new QGroupBox(tr("Iconsize"));
	iconSizeGroup = new QButtonGroup();	
	iconSizeSmall = new QRadioButton(tr("small"));
	iconSizeMedium =new QRadioButton(tr("medium"));
	iconSizeLarge = new QRadioButton(tr("large"));

	iconSizeGroup->addButton(iconSizeSmall,1);
	iconSizeGroup->addButton(iconSizeMedium,2);
	iconSizeGroup->addButton(iconSizeLarge,3);

	iconSizeBoxLayout = new QHBoxLayout();
	iconSizeBoxLayout->addWidget(iconSizeSmall);
	iconSizeBoxLayout->addWidget(iconSizeMedium);
	iconSizeBoxLayout->addWidget(iconSizeLarge);

	iconSizeBox->setLayout(iconSizeBoxLayout);

	/*****************************************
	 * Icon-Style-Box
	 *****************************************/
    iconStyleBox = new QGroupBox(tr("Iconstyle"));
	iconStyleGroup = new QButtonGroup();
	iconTextButton = new QRadioButton(tr("just text"));
	iconIconsButton =new QRadioButton(tr("just icons"));
	iconAllButton = new QRadioButton(tr("text and icons"));

	iconStyleGroup->addButton(iconTextButton,1);
	iconStyleGroup->addButton(iconIconsButton,2);
	iconStyleGroup->addButton(iconAllButton,3);

	iconStyleBoxLayout = new QHBoxLayout();
	iconStyleBoxLayout->addWidget(iconTextButton);
	iconStyleBoxLayout->addWidget(iconIconsButton);
	iconStyleBoxLayout->addWidget(iconAllButton);

	iconStyleBox->setLayout(iconStyleBoxLayout);	

	/*****************************************
	 * Window-Size-Box
	 *****************************************/
	windowSizeBox = new QGroupBox(tr("Windowstate"));
	windowSizeBoxLayout = new QHBoxLayout();
	windowSizeCheckBox =new QCheckBox(tr("save window size and position on exit"),this);
	windowSizeBoxLayout->addWidget(windowSizeCheckBox);
	windowSizeBox->setLayout(windowSizeBoxLayout);	

    /*****************************************
	 * Language Select Box
	 *****************************************/
    QGroupBox *langBox = new QGroupBox(tr("Language"));
    QHBoxLayout *hbox2 = new QHBoxLayout();
    QComboBox *langSelectBox = new QComboBox;
    
    QHash<QString, QString> lang = listLanguages();
    
    foreach(QString l , lang) {
        langSelectBox->addItem(l);
        //qDebug() << l;
    }
    
    hbox2->addWidget(langSelectBox);
    langBox->setLayout(hbox2);

	/*****************************************
	 * Button-Box
	 *****************************************/
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(applySettings()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	/*****************************************
	 * Main V-Box
	 *****************************************/
    vbox = new QVBoxLayout();
    vbox->addWidget(iconSizeBox);
	vbox->addWidget(iconStyleBox);
    vbox->addWidget(windowSizeBox);
    vbox->addWidget(langBox);
    vbox->addWidget(buttonBox);
    setLayout(vbox);
	
	setSettings();
    exec();
}

/**********************************
 * Read the settings from config 
 * and set the buttons and checkboxes
 * appropriately
 **********************************/
void SettingsDialog::setSettings()
{
	QSettings settings;
	
	//Iconsize
	QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
	switch (iconSize.height()){
	case 12: iconSizeSmall->setChecked(true);
		break;
    case 24:iconSizeMedium->setChecked(true);
		break;
    case 32:iconSizeLarge->setChecked(true);
		break;
	}
	
	// Iconstyle
	Qt::ToolButtonStyle iconStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
	switch (iconStyle){
	case Qt::ToolButtonTextOnly: iconTextButton->setChecked(true);
		break;
    case Qt::ToolButtonIconOnly:iconIconsButton->setChecked(true);
		break;
    case Qt::ToolButtonTextUnderIcon:iconAllButton->setChecked(true);
		break;
	default:
		break;
	}
	
	Qt::CheckState windowSave = static_cast<Qt::CheckState>(settings.value("window/windowSave", Qt::Unchecked).toUInt());
	windowSizeCheckBox->setCheckState(windowSave);
}

/***********************************
 * get the values of the buttons and
 * write them to settings-file
 *************************************/
void SettingsDialog::applySettings()
{
	 QSettings settings;
     //settings.setValue("geometry", saveGeometry());

	switch (iconSizeGroup->checkedId()){
	case 1: settings.setValue("toolbar/iconsize", QSize(12, 12));
		break;
    case 2:settings.setValue("toolbar/iconsize", QSize(24, 24));
		break;
    case 3:settings.setValue("toolbar/iconsize", QSize(32, 32));
		break;
	}

	switch (iconStyleGroup->checkedId()){
	case 1: settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextOnly);
		break;
    case 2:settings.setValue("toolbar/iconstyle", Qt::ToolButtonIconOnly);
		break;
    case 3:settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon);
		break;
	}

	settings.setValue("window/windowSave", windowSizeCheckBox->checkState());
	accept();
}

// http://www.informit.com/articles/article.aspx?p=1405555&seqNum=3
QHash<QString, QString> SettingsDialog::listLanguages() {
    
    // translate this String to language used, the language list gets 
    // filled from this
    QString(tr("English"));
    
    //QStringList languages;
    QHash<QString, QString> languages;
    
    QString appPath = qApp->applicationDirPath();
    QDir qmDir = QDir(appPath + "/ts/");
    QStringList fileNames =
            qmDir.entryList(QStringList("gpg4usb_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);
        
        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        QString language = translator.translate("SettingsDialog",
                                                "English");
        languages.insert(locale, language);
        
    }
    return languages;
}
