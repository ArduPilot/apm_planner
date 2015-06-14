
#include "QsLog.h"

#include "JoystickWidget.h"
#include "ui_JoystickWidget.h"

#include "UASManager.h"

JoystickWidget::JoystickWidget(JoystickInput* joystick, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::JoystickWidget)
{
    m_ui->setupUi(this);
	clearKeys();
    this->joystick = joystick;

    updateMappings();

    connect(this->joystick, SIGNAL(joystickChanged(double,double,double,double,int,int,int)), this, SLOT(updateJoystick(double,double,double,double,int,int,int)));
    connect(this->joystick, SIGNAL(xChanged(double)), this, SLOT(setX(double)));
    connect(this->joystick, SIGNAL(yChanged(double)), this, SLOT(setY(double)));
    connect(this->joystick, SIGNAL(yawChanged(double)), this, SLOT(setZ(double)));
    connect(this->joystick, SIGNAL(thrustChanged(double)), this, SLOT(setThrottle(double)));
    connect(this->joystick, SIGNAL(buttonPressed(int)), this, SLOT(pressKey(int)));
    connect(this->joystick, SIGNAL(hatDirectionChanged(int, int)), this, SLOT(setHat(int, int)));

    connect(m_ui->rollMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingXAxis(int)));
    connect(m_ui->pitchMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingYAxis(int)));
    connect(m_ui->yawMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingYawAxis(int)));
    connect(m_ui->throttleMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingThrustAxis(int)));

    connect(m_ui->rollInvertCheckBox, SIGNAL(clicked(bool)), this->joystick, SLOT(setYReversed(bool)));
    connect(m_ui->pitchInvertCheckBox, SIGNAL(clicked(bool)), this->joystick, SLOT(setXReversed(bool)));
    connect(m_ui->yawInvertCheckBox, SIGNAL(clicked(bool)), this->joystick, SLOT(setYawReversed(bool)));
    connect(m_ui->throttleInvertCheckBox, SIGNAL(clicked(bool)), this->joystick, SLOT(setThrustReversed(bool)));

    connect(m_ui->autoMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingAutoButton(int)));
    connect(m_ui->stabilizeMapSpinBox, SIGNAL(valueChanged(int)), this->joystick, SLOT(setMappingStabilizeButton(int)));

    connect(this, SIGNAL(accepted()), this, SLOT(storeMapping()));
    connect(this, SIGNAL(rejected()), this, SLOT(restoreMapping()));

    if (joystick)
    {
        connect(joystick, SIGNAL(joystickSelected(const QString&)),
                this, SLOT(joystickSelected(const QString&)));
        connect(m_ui->joystickButton, SIGNAL(clicked(bool)),
                this, SLOT(joystickEnabled(bool)));
        connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)),
                this, SLOT(activeUASSet(UASInterface*)));

        const QString& name = joystick->getName();
        if (name != "")
        {
            // m_ui->joystickButton->setText(name);
            joystickSelected(name);
        }
    }

    // Display the widget
    this->window()->setWindowTitle(tr("Joystick"));
    this->show();
}

JoystickWidget::~JoystickWidget()
{
    close();
    delete m_ui;
}

void JoystickWidget::joystickSelected(const QString& name)
{
    m_ui->joystickLabel->setText(name);
    m_ui->joystickButton->setEnabled(true);
    joystick->setActiveUAS(NULL);
}

void JoystickWidget::joystickEnabled(bool checked)
{
    if (checked)
    {
        QLOG_INFO() << "Enabling joystick";
        m_ui->joystickButton->setText("Deactivate");
        joystick->setActiveUAS(UASManager::instance()->getActiveUAS());
    }
    else
    {
        QLOG_INFO() << "Disabling joystick";
        m_ui->joystickButton->setText("Activate");
        joystick->setActiveUAS(NULL);
    }
}

void JoystickWidget::activeUASSet(UASInterface* uas)
{
    if (m_ui->joystickButton->isChecked())
        joystick->setActiveUAS(uas);
}

void JoystickWidget::updateMappings()
{
    m_ui->rollMapSpinBox->setValue(joystick->getMappingXAxis());
    m_ui->pitchMapSpinBox->setValue(joystick->getMappingYAxis());
    m_ui->yawMapSpinBox->setValue(joystick->getMappingYawAxis());
    m_ui->throttleMapSpinBox->setValue(joystick->getMappingThrustAxis());

    m_ui->rollInvertCheckBox->setChecked(joystick->getXReversed());
    m_ui->pitchInvertCheckBox->setChecked(joystick->getYReversed());
    m_ui->yawInvertCheckBox->setChecked(joystick->getYawReversed());
    m_ui->throttleInvertCheckBox->setChecked(joystick->getThrustReversed());

    m_ui->autoMapSpinBox->setValue(joystick->getMappingAutoButton());
    m_ui->stabilizeMapSpinBox->setValue(joystick->getMappingStabilizeButton());
}

void JoystickWidget::storeMapping()
{
    joystick->storeSettings();
}

void JoystickWidget::restoreMapping()
{
    joystick->loadSettings();
    updateMappings();
}

void JoystickWidget::updateJoystick(double roll, double pitch, double yaw, double thrust, int xHat, int yHat, int buttons)
{
    setX(pitch);
    setY(roll);
    setZ(yaw);
    setThrottle(thrust);
    setHat(xHat, yHat);

    for (int i = 0; i < 11; ++i)
    {
        if (buttons & (1<<i))
            pressKey(i);
    }
}

void JoystickWidget::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
    QDialog::changeEvent(e);
}


void JoystickWidget::setThrottle(double thrust)
{
    m_ui->thrust->setValue(thrust*100.0);
}

void JoystickWidget::setX(double x)
{
    if (isVisible() && m_ui->xValue->intValue() != (int)(x*100)) {
        QLOG_TRACE() << "X:" << x;
    }

    m_ui->xSlider->setValue(x*100.0);
    m_ui->xValue->display((int)(x*100));
}

void JoystickWidget::setY(double y)
{
    if (isVisible() && m_ui->yValue->intValue() != (int)(y*100)) {
        QLOG_TRACE() << "Y:" << y;
    }

    m_ui->ySlider->setValue(y*100.0);
    m_ui->yValue->display((int)(y*100));
}

void JoystickWidget::setZ(double z)
{
    if (isVisible() && (int)m_ui->dial->value() != (int)z) {
        QLOG_TRACE() << "Z:" << z;
    }

    m_ui->dial->setValue(z*100.0);
}

void JoystickWidget::setHat(int x, int y)
{
    updateStatus(tr("Hat position: x: %1, y: %2").arg(x).arg(y));
}

void JoystickWidget::clearKeys()
{
    QString colorstyle;
    QColor buttonStyleColor = QColor(200, 20, 20);
    colorstyle = QString("QLabel { border: 1px solid #EEEEEE; border-radius: 4px; padding: 0px; margin: 0px; background-color: %1;}").arg(buttonStyleColor.name());

    m_ui->button0->setStyleSheet(colorstyle);
    m_ui->button1->setStyleSheet(colorstyle);
    m_ui->button2->setStyleSheet(colorstyle);
    m_ui->button3->setStyleSheet(colorstyle);
    m_ui->button4->setStyleSheet(colorstyle);
    m_ui->button5->setStyleSheet(colorstyle);
    m_ui->button6->setStyleSheet(colorstyle);
    m_ui->button7->setStyleSheet(colorstyle);
    m_ui->button8->setStyleSheet(colorstyle);
    m_ui->button9->setStyleSheet(colorstyle);
    m_ui->button10->setStyleSheet(colorstyle);
}

void JoystickWidget::pressKey(int key)
{
    QString colorstyle;
    QColor buttonStyleColor = QColor(20, 200, 20);
    colorstyle = QString("QLabel { border: 1px solid #EEEEEE; border-radius: 4px; padding: 0px; margin: 0px; background-color: %1;}").arg(buttonStyleColor.name());
    switch(key) {
    case 0:
        m_ui->button0->setStyleSheet(colorstyle);
        break;
    case 1:
        m_ui->button1->setStyleSheet(colorstyle);
        break;
    case 2:
        m_ui->button2->setStyleSheet(colorstyle);
        break;
    case 3:
        m_ui->button3->setStyleSheet(colorstyle);
        break;
    case 4:
        m_ui->button4->setStyleSheet(colorstyle);
        break;
    case 5:
        m_ui->button5->setStyleSheet(colorstyle);
        break;
    case 6:
        m_ui->button6->setStyleSheet(colorstyle);
        break;
    case 7:
        m_ui->button7->setStyleSheet(colorstyle);
        break;
    case 8:
        m_ui->button8->setStyleSheet(colorstyle);
        break;
    case 9:
        m_ui->button9->setStyleSheet(colorstyle);
        break;
    case 10:
        m_ui->button10->setStyleSheet(colorstyle);
        break;
    }
    QTimer::singleShot(100, this, SLOT(clearKeys()));
    updateStatus(tr("Key %1 pressed").arg(key));
}

void JoystickWidget::updateStatus(const QString& status)
{
    m_ui->statusLabel->setText(status);
}
