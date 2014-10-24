#include "gstreamertoolbarwidget.h"
#include <QVBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QSettings>

// GStreamerToolBarWidget

void GStreamerToolBarWidget::createLayout()
{
     QGridLayout *layout = new QGridLayout;
     QSlider *brightness = new QSlider(Qt::Horizontal);
     brightness->setRange(-100, 100);
     QLabel *brightnessLabel = new QLabel("Brightness");
     QSlider *contrast = new QSlider(Qt::Horizontal);
     contrast->setRange(-100, 100);
     QLabel *contrastLabel = new QLabel("Contrast");

     QSlider *hue = new QSlider(Qt::Horizontal);
     hue->setRange(-100, 100);
     QLabel *hueLabel = new QLabel("Hue");
     QSlider *sat = new QSlider(Qt::Horizontal);
     sat->setRange(-100, 100);
     QLabel *satLabel = new QLabel("Saturation");

     layout->addWidget(brightnessLabel,0,0);
     layout->addWidget(brightness,0,1);
     layout->addWidget(contrastLabel,1,0);
     layout->addWidget(contrast,1,1);
     layout->addWidget(hueLabel,2,0);
     layout->addWidget(hue,2,1);
     layout->addWidget(satLabel,3,0);
     layout->addWidget(sat,3,1);
     setLayout(layout);

     brightness->setValue(m_player->getBrightness());
     contrast->setValue(m_player->getContrast());
     hue->setValue(m_player->getHue());
     sat->setValue(m_player->getSaturation());

     connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(brightnessValueChanged(int)));
     connect(contrast, SIGNAL(valueChanged(int)), this, SLOT(contrastValueChanged(int)));
     connect(hue, SIGNAL(valueChanged(int)), this, SLOT(hueValueChanged(int)));
     connect(sat, SIGNAL(valueChanged(int)), this, SLOT(satValueChanged(int)));
}

void GStreamerToolBarWidget::brightnessValueChanged(int value)
{
    m_player->setBrightness(value);

    QSettings settings;
    settings.beginGroup("QGC_GSTREAMER");
    settings.setValue("GSTREAMER_BRIGHTNESS", value);
    settings.endGroup();
    settings.sync();
}

void GStreamerToolBarWidget::contrastValueChanged(int value)
{
    m_player->setContrast(value);

    QSettings settings;
    settings.beginGroup("QGC_GSTREAMER");
    settings.setValue("GSTREAMER_CONTRAST", value);
    settings.endGroup();
    settings.sync();
}

void GStreamerToolBarWidget::hueValueChanged(int value)
{
    m_player->setHue(value);

    QSettings settings;
    settings.beginGroup("QGC_GSTREAMER");
    settings.setValue("GSTREAMER_HUE", value);
    settings.endGroup();
    settings.sync();
}

void GStreamerToolBarWidget::satValueChanged(int value)
{
    m_player->setSaturation(value);

    QSettings settings;
    settings.beginGroup("QGC_GSTREAMER");
    settings.setValue("GSTREAMER_SATURATION", value);
    settings.endGroup();
    settings.sync();
}
