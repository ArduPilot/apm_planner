
#include "QsLog.h"

#include "JoystickWidget.h"
#include "ui_JoystickWidget.h"

#include "UASManager.h"


JoystickWidget::JoystickWidget(JoystickInput* joystick, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::JoystickWidget),
    m_buttonPressedMessage("")
{
    m_ui->setupUi(this);

    for (int i = 0; i < joystick->getNumberOfButtons(); ++i) {
        addJoystickButtonLabel(i);
    }

    clearKeys();
    this->joystick = joystick;

    updateMappings();

    connect(this->joystick, SIGNAL(joystickChanged(double,double,double,double,int,int,int)), this, SLOT(updateJoystick(double,double,double,double,int,int,int)));
    connect(this->joystick, SIGNAL(xChanged(double)), this, SLOT(setX(double)));
    connect(this->joystick, SIGNAL(yChanged(double)), this, SLOT(setY(double)));
    connect(this->joystick, SIGNAL(yawChanged(double)), this, SLOT(setZ(double)));
    connect(this->joystick, SIGNAL(thrustChanged(double)), this, SLOT(setThrottle(double)));
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

void JoystickWidget::addJoystickButtonLabel(int i)
{
    QLabel* button = new QLabel(m_ui->groupBox);
    button->setObjectName("button" + QString::number(i));
    button->setEnabled(true);
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(button->sizePolicy().hasHeightForWidth());
    button->setSizePolicy(sizePolicy);
    button->setAlignment(Qt::AlignCenter);
    button->setText(QString::number(i));
    m_ui->verticalLayout->addWidget(button);
    m_buttonList.append(button);
    m_buttonStates[i] = false;
}

void JoystickWidget::joystickSelected(const QString& name)
{
    m_ui->joystickLabel->setText(name);
    m_ui->joystickButton->setEnabled(true);
    joystick->setActiveUAS(NULL);
    updateMappings();

    m_buttonList.clear();
    for (int i = 0; i < joystick->getNumberOfButtons(); ++i) {
        addJoystickButtonLabel(i);
    }
    clearKeys();
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

    for (int i = 0; i < m_buttonList.size(); ++i)
    {
        if (m_buttonStates[i] != (buttons & (1<<i))) {
            buttonStateChanged(i, (buttons & (1<<i)));
            m_buttonStates[i] = (buttons & (1<<i));
        }
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
    updateStatus(m_buttonPressedMessage + " " + tr("Hat position: x: %1, y: %2").arg(x).arg(y));
}

void JoystickWidget::clearKeys()
{
    QString colorstyle;
    QColor buttonStyleColor = QColor(200, 20, 20);
    colorstyle = QString("QLabel { border: 1px solid #EEEEEE; border-radius: 4px; padding: 0px; margin: 0px; background-color: %1;}").arg(buttonStyleColor.name());

    for (int i = 0; i < m_buttonList.size(); ++i) {
        m_buttonList[i]->setStyleSheet(colorstyle);
    }
}

void JoystickWidget::buttonStateChanged(const int key, const bool pressed)
{
    QString colorstyle;
    QColor buttonStyleColor = pressed ? QColor(20, 200, 20) : QColor(200, 20, 20);
    colorstyle = QString("QLabel { border: 1px solid #EEEEEE; border-radius: 4px; padding: 0px; margin: 0px; background-color: %1;}").arg(buttonStyleColor.name());

    if (key < m_buttonList.size()) {
        m_buttonList[key]->setStyleSheet(colorstyle);
    }

    if (pressed) {
        m_buttonPressedMessage = tr("Key %1 pressed").arg(key);
    } else {
        m_buttonPressedMessage = "";
    }
}

void JoystickWidget::updateStatus(const QString& status)
{
    m_ui->statusLabel->setText(status);
}
