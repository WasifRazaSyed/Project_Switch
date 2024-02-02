#include "settings_widget.h"

settings_widget::settings_widget(QWidget *parent)
    : QWidget{parent}
{
    loadsettings();

    int id = QFontDatabase::addApplicationFont("://font/Nunito-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont steel(family);
    steel.setPointSize(10);
    setObjectName("settings");

    QLabel *max=new QLabel(this);
    QLabel *min=new QLabel(this);
    QLabel *mac=new QLabel(this);
    QLabel *note=new QLabel(this);
    note->setObjectName("note");
    QPushButton *neg1=new QPushButton(this);
    QPushButton *neg2=new QPushButton(this);
    QPushButton *pos1=new QPushButton(this);
    QPushButton *pos2=new QPushButton(this);
    QPushButton *info=new QPushButton(this);
    QPushButton *save=new QPushButton(this);

    max_slider=new QSlider(Qt::Horizontal,this);
    min_slider=new QSlider(Qt::Horizontal,this);
    key=new serial_key(this);
    QGridLayout *settings_layout=new QGridLayout(this);
    settings_layout->setContentsMargins(15,10,15,10);

    settings_layout->addWidget(min, 0, 2, 1, 3);
    settings_layout->addWidget(neg1, 1, 0, 1, 1);
    settings_layout->addWidget(min_slider, 1, 1, 1, 5);
    settings_layout->addWidget(pos1, 1, 6, 1,1);

    settings_layout->addWidget(max, 2, 2, 1, 3);
    settings_layout->addWidget(neg2, 3, 0, 1, 1);
    settings_layout->addWidget(max_slider, 3, 1, 1, 5);
    settings_layout->addWidget(pos2, 3, 6, 1,1);

    settings_layout->addWidget(mac, 4, 2, 1, 3);
    settings_layout->addWidget(key, 5, 2, 1, 5);
    settings_layout->addWidget(info, 5, 5, 1, 1);
    settings_layout->addWidget(save, 6, 3, 1, 1);
    settings_layout->addWidget(note, 7, 0, 2, 7);

    key->setFixedHeight(this->size().height()/1.2);
    key->setFixedWidth(this->size().width()/0.555);
    pos1->setFixedSize(28,28);
    pos2->setFixedSize(28,28);
    neg1->setFixedSize(28,28);
    neg2->setFixedSize(28,28);
    info->setFixedSize(28,28);
    key->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    key->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    min_slider->setMinimum(10);
    max_slider->setMinimum(30);
    min_slider->setMaximum(100);
    max_slider->setMaximum(100);
    QString min_text;
    min_text.append("Min Threshold: " + min_value.toString());
    min->setText(min_text);

    QString max_text;
    max_text.append("Max Threshold: " + max_value.toString());
    max->setText(max_text);

    min_slider->setValue(min_value.toInt());
    max_slider->setValue(max_value.toInt());
    mac->setText("Serial Key");
    save->setText("Save");
    note->setText("Note: For better battery life, we recommend you to have \nbattery bracket of 30-80.");

    min->setAlignment(Qt::AlignCenter);
    max->setAlignment(Qt::AlignCenter);
    mac->setAlignment(Qt::AlignCenter);
    key->setAlignment(Qt::AlignCenter);

    key->setObjectName("mac_edit");
    pos1->setObjectName("pos1");
    pos2->setObjectName("pos2");
    neg1->setObjectName("neg1");
    neg2->setObjectName("neg2");
    save->setObjectName("save");
    info->setObjectName("info");
    min->setFont(steel);
    max->setFont(steel);
    mac->setFont(steel);
    key->setFont(steel);
    note->setFont(steel);
    save->setFont(steel);

    connect(info, &QPushButton::pressed, this, [=](){
        QMessageBox::information(this, "One Time Input", "Please find 12-digit serial key behind your smart switch. \n e.g. ff8f1baaec65\n\n- This is required for once.\n- Make sure to have no typo.\n- This key is mandatory for smart switch to work properly.", QMessageBox::Ok);
    });

    connect(min_slider, &QSlider::valueChanged, this, [=](){

        QString min_text;
        QString min_value=QVariant(min_slider->value()).toString();
        min_text.append("Min Threshold: " + min_value);
        min->setText(min_text);

    });
    connect(max_slider, &QSlider::valueChanged,this,  [=](){

        QString max_text;
        QString max_value=QVariant(max_slider->value()).toString();
        max_text.append("Max Threshold: " + max_value);
        max->setText(max_text);

    });
    connect(neg1, &QPushButton::pressed, this, [=](){
        min_slider->setValue(min_slider->value()-1);
        QTimer *time=new QTimer(this);
        time->start(300);
        connect(time, &QTimer::timeout, this, [=](){
            min_slider->setValue(min_slider->value()-1);
        });
        connect(neg1, &QPushButton::released, this, [=](){
            time->stop();
        });
    });
    connect(neg2, &QPushButton::pressed, this, [=](){
        max_slider->setValue(max_slider->value()-1);
        QTimer *time=new QTimer(this);
        time->start(300);
        connect(time, &QTimer::timeout, this, [=](){
            max_slider->setValue(max_slider->value()-1);
        });
        connect(neg2, &QPushButton::released, this, [=](){
            time->stop();
        });

    });
    connect(pos1, &QPushButton::pressed, this, [=](){
        min_slider->setValue(min_slider->value()+1);
        QTimer *time=new QTimer(this);
        time->start(300);
        connect(time, &QTimer::timeout,this,  [=](){
            min_slider->setValue(min_slider->value()+1);
        });
        connect(pos1, &QPushButton::released,this,  [=](){
            time->stop();
        });
    });
    connect(pos2, &QPushButton::pressed,this,  [=](){
        max_slider->setValue(max_slider->value()+1);
        QTimer *time=new QTimer(this);
        time->start(300);
        connect(time, &QTimer::timeout,this,  [=](){
            max_slider->setValue(max_slider->value()+1);
        });
        connect(pos2, &QPushButton::released, this, [=](){
            time->stop();
        });
    });
    connect(save, &QPushButton::pressed,this, [=](){
        if(min_slider->value()>=max_slider->value()){
            QMessageBox::critical(this, "Invalid Value", "Please select appropriate values. Max-threshold must be greater than Min-threshold.", QMessageBox::Ok);
        }
        else if((key->toPlainText()!="" && (key->toPlainText().length()>12 || key->toPlainText().length()<12)) || (key->toPlainText()=="" && check==false))
        {
            QMessageBox::critical(this, "Invalid Serial", "Please type in serial key correctly. Press  !  icon next to serial field for more info.", QMessageBox::Ok);
        }
        else if(key->toPlainText().isEmpty()&&serial_key_.isEmpty()){
            QMessageBox::critical(this, "Empty serial", "Serial key is mandatory, Press  !  icon next to serial field for more info.", QMessageBox::Ok);
        }
        else{
            savesettings();
            QMessageBox::information(this, "Smart Act", "Changes are saved.", QMessageBox::Ok);
        }
    });

}

void settings_widget::savesettings()
{
    QString location=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/settings.ini";
    QSettings private_ini(location,QSettings::IniFormat);
    QSettings private_("smart_switch", "smart_settings");
    private_.beginGroup("threshold");
    private_.setValue("max_threshold", max_slider->value());
    private_.setValue("min_threshold", min_slider->value());
    private_.endGroup();
    private_.beginGroup("flag");
    private_.setValue("used", true);
    private_.endGroup();

    private_ini.beginGroup("threshold");
    private_ini.setValue("max_threshold", max_slider->value());
    private_ini.setValue("min_threshold", min_slider->value());
    private_ini.endGroup();
    private_ini.beginGroup("flag");
    private_ini.setValue("used", true);
    private_ini.setValue("change", true);
    private_ini.endGroup();
    if(key->toPlainText().length()==12)
    {
    private_.beginGroup("mac");
    private_.setValue("MAC", key->toPlainText());
    private_.endGroup();
    private_ini.beginGroup("mac");
    private_ini.setValue("MAC", key->toPlainText());
    private_ini.endGroup();
    }
    return;
}

void settings_widget::loadsettings()
{
    QSettings private_("smart_switch", "smart_settings");
    QString location=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/settings.ini";
    QSettings private_ini(location,QSettings::IniFormat);

    private_.beginGroup("flag");
    check=private_.value("used").toBool();
    private_.endGroup();

    if(check==true){
    private_.beginGroup("threshold");
    max_value=private_.value("max_threshold").toString();
    min_value=private_.value("min_threshold").toString();
    private_.endGroup();

    private_.beginGroup("mac");
    serial_key_=private_.value("MAC").toString();
    private_.endGroup();

    }
    else{

        private_ini.beginGroup("threshold");
        max_value=private_ini.value("max_threshold").toString();
        min_value=private_ini.value("min_threshold").toString();
        private_ini.endGroup();
        private_ini.beginGroup("mac");
        serial_key_=private_ini.value("MAC").toString();
        private_ini.endGroup();
        private_ini.beginGroup("flag");
        check=private_ini.value("used").toBool();
        private_ini.endGroup();
        if(check!=true){
            max_value="80";
            min_value="30";
        }
        else{
            private_ini.beginGroup("mac");
            private_.beginGroup("mac");
            private_.setValue("MAC",serial_key_);
            private_.setValue("version", private_ini.value("version").toString());
            private_.endGroup();
            private_ini.endArray();

            private_.beginGroup("flag");
            private_.setValue("used",check);
            private_.endGroup();

            private_.beginGroup("threshold");
            private_.setValue("max_threshold",max_value);
            private_.setValue("min_threshold",min_value);
            private_.endGroup();
        }
    }
    return;
}

void settings_widget::exit_check()
{
    QString location=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/settings.ini";
    QSettings private_ini(location,QSettings::IniFormat);
    QSettings private_("smart_switch", "smart_settings");
    private_.beginGroup("threshold");
    max_value=private_.value("max_threshold").toString();
    min_value=private_.value("min_threshold").toString();
    private_.endGroup();
    if(max_value==0||min_value==0){
        private_ini.beginGroup("threshold");
        max_value=private_ini.value("max_threshold").toString();
        min_value=private_ini.value("min_threshold").toString();
        private_ini.endGroup();
    }

    private_.beginGroup("flag");
    check=private_.value("used").toBool();
    private_.endGroup();
    if(check==false){
        private_ini.beginGroup("flag");
        check=private_ini.value("used").toBool();
        private_ini.endGroup();
    }
    private_.beginGroup("mac");
    serial_key_=private_.value("MAC").toString();
    private_.endGroup();
    if(serial_key_.isEmpty()){

        private_ini.beginGroup("mac");
        serial_key_=private_ini.value("MAC").toString();
        private_ini.endGroup();
        private_.beginGroup("mac");
        private_.setValue("MAC", serial_key_);
        private_.endGroup();
    }



    if(max_slider->value()!=max_value||min_slider->value()!=min_value || (key->toPlainText()!=serial_key_ &&key->toPlainText()!=""))
    {
        int ret=QMessageBox::question(this, "Save Changes", "Changes were made. Press Yes to save the changes.", QMessageBox::Yes, QMessageBox::No);
        if(ret==0x00004000)
        {
            if(min_slider->value()>=max_slider->value()){
                QMessageBox::critical(this, "Invalid Value", "Please select appropriate values. Max-threshold must be greater than Min-threshold.", QMessageBox::Ok);
            }
            else if(key->toPlainText().length()>12 || key->toPlainText().length()<12 || (key->toPlainText()=="" && check==false)){
                QMessageBox::critical(this, "Invalid Serial", "Please type in serial key correctly. Press  !  icon next to serial field for more info.", QMessageBox::Ok);
            }
            else{
                savesettings();
            int ret1=QMessageBox::information(this, "Smart Act", "Changes are saved.", QMessageBox::Ok);
            if(ret1==0x00000400)
            {
                QApplication::quit();
            }
            }
        }
        else if(ret==0x00010000){
            QApplication::quit();
        }
    }
    else{
        QApplication::quit();
    }
}
