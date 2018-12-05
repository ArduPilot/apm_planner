/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2013 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Implementation of class MainWindow
 *   @author Lorenz Meier <mail@qgroundcontrol.org>
 */
#include "logging.h"
#include "dockwidgettitlebareventfilter.h"
#include "QGC.h"
#include "MAVLinkSimulationLink.h"
#include "UDPLink.h"
#include "MAVLinkProtocol.h"
#include "CommConfigurationWindow.h"
#include "QGCWaypointListMulti.h"
#include "MainWindow.h"
#include "JoystickWidget.h"
#include "GAudioOutput.h"
#include "QGCToolWidget.h"
#include "QGCMAVLinkLogPlayer.h"
#include "QGCSettingsWidget.h"
#include "QGCMapTool.h"
#include "MAVLinkDecoder.h"
#include "QGCRGBDView.h"
#include "QGCFirmwareUpdate.h"
#include "QGCStatusBar.h"
#include "UASQuickView.h"
#include "UASActionsWidget.h"
#include "QGCTabbedInfoView.h"
#include "UASRawStatusView.h"
#include "PrimaryFlightDisplay.h"
#include "PrimaryFlightDisplayQML.h"
#include "VibrationMonitor.h"
#include "EKFMonitor.h"
#include "ApmToolBar.h"
#include "SerialSettingsDialog.h"
#include "TerminalConsole.h"
#include "AP2DataPlot2D.h"
#include "MissionElevationDisplay.h"
#include "LinkManagerFactory.h"

#ifdef QGC_OSG_ENABLED
#include "Q3DWidgetFactory.h"
#endif

#include "AboutDialog.h"

// FIXME Move
#include "PxQuadMAV.h"
#include "SlugsMAV.h"
#include "LogCompressor.h"

#include <QSettings>
#include <QDockWidget>
#include <QNetworkInterface>
#include <QMessageBox>

#include <QTimer>
#include <QHostInfo>
#include <QSplashScreen>
#include <QGCHilLink.h>
#include <QGCHilConfiguration.h>
#include <QGCHilFlightGearConfiguration.h>

#define PFD_QML


LogWindowSingleton &LogWindowSingleton::instance()
{
   static LogWindowSingleton instance;
   return instance;
}

void LogWindowSingleton::write(const QString &message)
{
   if (!m_debugPtr.isNull())
   {
      if (!m_outPutBuffer.empty())
      {
         foreach (QString string, m_outPutBuffer)
         {
            m_debugPtr->write(string);
         }
         m_outPutBuffer.clear();
         m_startupBuffering = false;
      }
      m_debugPtr->write(message);
   }
   else if (m_startupBuffering)
   {
      m_outPutBuffer.append(message);
   }
}

void LogWindowSingleton::setDebugOutput(DebugOutput::Ptr outputPtr)
{
   m_debugPtr = outputPtr;
}

void LogWindowSingleton::removeDebugOutput()
{
   m_debugPtr.clear();
}


MainWindow* MainWindow::instance(QSplashScreen* screen)
{
    static MainWindow* _instance = 0;
    if(_instance == 0)
    {
        _instance = new MainWindow();
        if (screen) connect(_instance, SIGNAL(initStatusChanged(QString)), screen, SLOT(showMessage(QString)));

        /* Set the application as parent to ensure that this object
                 * will be destroyed when the main application exits */
        //_instance->setParent(qApp);
    }
    return _instance;
}

// inline function definitions

int MainWindow::getStyle()
{
    return currentStyle;
}

bool MainWindow::autoReconnectEnabled()
{
    return autoReconnect;
}

bool MainWindow::dockWidgetTitleBarsEnabled()
{
    return dockWidgetTitleBarEnabled;
}

bool MainWindow::lowPowerModeEnabled()
{
    return lowPowerMode;
}

bool MainWindow::autoProxyModeEnabled()
{
    return autoProxyMode;
}

/**
* Create new mainwindow. The constructor instantiates all parts of the user
* interface. It does NOT show the mainwindow. To display it, call the show()
* method.
*
* @see QMainWindow::show()
**/
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    aboutToCloseFlag(false),
    changingViewsFlag(false),
    centerStackActionGroup(new QActionGroup(this)),
    styleFileName(QCoreApplication::applicationDirPath() + "/style-outdoor.css"),
    m_heartbeatEnabled(true),
    m_terminalDialog(NULL)
{
    QLOG_DEBUG() << "Creating MainWindow";
    this->setAttribute(Qt::WA_DeleteOnClose);
    hide();
    emit initStatusChanged("Loading UI Settings..");
    loadSettings();
    enableDockWidgetTitleBars(dockWidgetTitleBarEnabled);

    emit initStatusChanged("Loading Style.");
    loadStyle(currentStyle);

    emit initStatusChanged("Setting up user interface.");

    // Setup user interface
    ui.setupUi(this);
    hide();

    ui.actionAdvanced_Mode->setChecked(isAdvancedMode);
    ui.actionSimulate->setVisible(false);

    // We only need this menu if we have more than one system
    //    ui.menuConnected_Systems->setEnabled(false);

    // Set dock options
    setDockOptions(AnimatedDocks | AllowTabbedDocks | AllowNestedDocks);

    configureWindowName();

    // Setup corners
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    // Setup UI state machines
    centerStackActionGroup->setExclusive(true);

    centerStack = new QStackedWidget(this);
    setCentralWidget(centerStack);


    // Load Toolbar
#ifdef QGC_TOOLBAR_ENABLED
    toolBar = new QGCToolBar(this);
    this->addToolBar(toolBar);

    // Add actions for average users (displayed next to each other)
    QList<QAction*> actions;
    actions << ui.actionFlightView;
    actions << ui.actionMissionView;
    //actions << ui.actionConfiguration_2;
    actions << ui.actionHardwareConfig;
    actions << ui.actionSoftwareConfig;
    toolBar->setPerspectiveChangeActions(actions);

    // Add actions for advanced users (displayed in dropdown under "advanced")
    QList<QAction*> advancedActions;
    advancedActions << ui.actionSimulation_View;
    advancedActions << ui.actionEngineersView;

    toolBar->setPerspectiveChangeAdvancedActions(advancedActions);
#endif

    customStatusBar = new QGCStatusBar(this);
    setStatusBar(customStatusBar);
    statusBar()->setSizeGripEnabled(true);
    statusBar()->hide();

    emit initStatusChanged("Building common widgets.");

    buildCommonWidgets();
    connectCommonWidgets();

    emit initStatusChanged("Building common actions.");

    // Create actions
    connectCommonActions();

    // Populate link menu
    emit initStatusChanged("Populating link menu");

    QList<int> links = LinkManager::instance()->getLinks();
    for (int i=0;i<links.size();i++)
    {
        addLink(links.at(i));
    }

    connect(LinkManager::instance(), SIGNAL(newLink(int)), this, SLOT(addLink(int)), Qt::QueuedConnection);
    connect(LinkManager::instance(),SIGNAL(linkError(int,QString)),this,SLOT(linkError(int,QString)));

    connect(ui.actionTerminalConsole, SIGNAL(triggered()), this, SLOT(showTerminalConsole()));

#ifndef QGC_TOOLBAR_ENABLED
    // Add the APM 'toolbar'

    m_apmToolBar = new APMToolBar();
    m_apmToolBar->setFlightViewAction(ui.actionFlightView);
    m_apmToolBar->setFlightPlanViewAction(ui.actionMissionView);
    m_apmToolBar->setInitialSetupViewAction(ui.actionHardwareConfig);
    m_apmToolBar->setConfigTuningViewAction(ui.actionSoftwareConfig);
    m_apmToolBar->setPlotViewAction(ui.actionEngineersView);
    m_apmToolBar->setSimulationViewAction(ui.actionSimulation_View);
    m_apmToolBar->setDonateViewAction(ui.actionDonate);

    connect(ui.actionAdvanced_Mode, SIGNAL(triggered(bool)), m_apmToolBar, SLOT(checkAdvancedMode(bool)));

    QDockWidget *widget = new QDockWidget(tr("APM Tool Bar"),this);
    QWidget *toolbarcontainer = QWidget::createWindowContainer(m_apmToolBar);
    widget->setWidget(toolbarcontainer);
    widget->setMinimumHeight(72);
    widget->setMaximumHeight(72);
    widget->setMinimumWidth(1024);
    widget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    widget->setTitleBarWidget(new QWidget(this)); // Disables the title bar
    this->addDockWidget(Qt::TopDockWidgetArea, widget);
#endif

    // Connect user interface devices
    emit initStatusChanged("Initializing joystick interface.");
    joystickWidget = 0;
    joystick = new JoystickInput();

#ifdef MOUSE_ENABLED_WIN
    emit initStatusChanged("Initializing 3D mouse interface.");

    mouseInput = new Mouse3DInput(this);
    mouse = new Mouse6dofInput(mouseInput);
#endif //MOUSE_ENABLED_WIN

#if MOUSE_ENABLED_LINUX
    emit initStatusChanged("Initializing 3D mouse interface.");

    mouse = new Mouse6dofInput(this);
    connect(this, SIGNAL(x11EventOccured(XEvent*)), mouse, SLOT(handleX11Event(XEvent*)));
#endif //MOUSE_ENABLED_LINUX

    // Set low power mode
    enableLowPowerMode(lowPowerMode);

    // Set Automatic use of system Proxies
    enableAutoProxyMode(autoProxyMode);

    // Initialize window state
    windowStateVal = windowState();

    emit initStatusChanged("Restoring last view state.");

    // Restore the window setup
    loadViewState();

    emit initStatusChanged("Restoring last window size.");
    // Restore the window position and size
    if (settings.contains(getWindowGeometryKey()))
    {
        // Restore the window geometry
        restoreGeometry(settings.value(getWindowGeometryKey()).toByteArray());
        show();
    }
    else
    {
        // Adjust the size
        QDesktopWidget* desktopWidget = qApp->desktop();
        static QRect rect = desktopWidget->screenGeometry();
        QLOG_INFO() << "Screen Size is " << rect;
        const int screenWidth = rect.width();
        const int screenHeight = rect.height();

        if (screenWidth < 1500)
        {
            resize(screenWidth, screenHeight - 80);
            show();
        }
        else
        {
            resize(screenWidth*0.67f, qMin(screenHeight, (int)(screenWidth*0.67f*0.67f)));
            show();
        }

    }

    connect(&windowNameUpdateTimer, SIGNAL(timeout()), this, SLOT(configureWindowName()));
    windowNameUpdateTimer.start(15000);
    emit initStatusChanged("Done.");

    ui.actionDeveloper_Credits->setVisible(false);
    ui.actionOnline_Documentation->setVisible(false);
    ui.actionProject_Roadmap_2->setVisible(false);
    show();


    // Disable firmware update and unconnected view buttons, as they aren't required for the moment.
    ui.actionFirmwareUpdateView->setVisible(false);
    ui.actionUnconnectedView->setVisible(false);

    // in Linux, query if we have the correct permissions to
    // access USB serial devices
#ifdef Q_OS_LINUX
    QFile permFile("/etc/group");
    if(permFile.open(QIODevice::ReadOnly))
    {
        while(!permFile.atEnd())
        {
            QString line = permFile.readLine();
            if(line.contains("dialout") && !line.contains(getenv("USER")))
            {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setInformativeText(tr("The current user does not have the correct permissions to access serial devices. Use \"sudo usermod -a -G dialout $USER\" and then log out and in again"));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();
                QLOG_INFO() << "User does not have permissions to serial devices";
                break;
            }
        }
        permFile.close();
    }
#endif

    // Trigger Auto Update Check
    m_autoUpdateCheck.suppressNoUpdateSignal();
    QTimer::singleShot(5000, &m_autoUpdateCheck, SLOT(autoUpdateCheck()));
    connect(&m_autoUpdateCheck, SIGNAL(updateAvailable(QString,QString,QString,QString)),
            this, SLOT(showAutoUpdateDownloadDialog(QString,QString,QString,QString)));
    connect(&m_autoUpdateCheck, SIGNAL(noUpdateAvailable()),
            this, SLOT(showNoUpdateAvailDialog()));

    settings.endGroup();

}

MainWindow::~MainWindow()
{
    closeTerminalConsole();

    if (joystickWidget)
    {
        QLOG_DEBUG() << "Delete JoystickWidget";

        delete joystickWidget;
        joystickWidget = NULL;
    }
    if (joystick)
    {
        joystick->shutdown();
        joystick->wait(5000);
        delete joystick;
        joystick = NULL;
    }

    // Get and delete all dockwidgets and contained
    // widgets
    QObjectList childList(this->children());

    QObjectList::iterator i;
    QDockWidget* dockWidget;
    for (i = childList.begin(); i != childList.end(); ++i)
    {
        dockWidget = dynamic_cast<QDockWidget*>(*i);
        if (dockWidget)
        {
            // Remove dock widget from main window
            // removeDockWidget(dockWidget);
            // delete dockWidget->widget();
            QLOG_DEBUG() << "Delete DockWidget " << dockWidget;
            delete dockWidget;
            dockWidget = NULL;
        }
        else if (dynamic_cast<QWidget*>(*i)) // [ToDo] Stability
        {
            QWidget* widget = dynamic_cast<QWidget*>(*i);
            QLOG_DEBUG() << "Delete Widget " << widget;
            delete widget;
            *i = NULL;
        }
    }
    // Delete all UAS objects
    for (int i=0;i<commsWidgetList.size();i++)
    {
        commsWidgetList[i]->deleteLater();
    }

    // Force the singleton to release the debug widget
    LogWindowSingleton::instance().removeDebugOutput();
}

void MainWindow::disableTLogReplayBar()
{
    statusBar()->hide();
}

void MainWindow::enableTLogReplayBar()
{
    statusBar()->show();
}

void MainWindow::loadTlogMenuClicked()
{
    return;
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
}

QString MainWindow::getWindowStateKey()
{
    if (UASManager::instance()->getActiveUAS())
    {
        return QString::number(currentView)+"_windowstate_" + UASManager::instance()->getActiveUAS()->getAutopilotTypeName();
    }
    else
        return QString::number(currentView)+"_windowstate";
}

QString MainWindow::getWindowGeometryKey()
{
    return "_geometry";
}

void MainWindow::buildCustomWidget()
{
    // Create custom widgets
    QList<QGCToolWidget*> widgets = QGCToolWidget::createWidgetsFromSettings(this);

    if (widgets.size() > 0)
    {
        ui.menuTools->addSeparator();
    }

    for(int i = 0; i < widgets.size(); ++i)
    {
        // Check if this widget already has a parent, do not create it in this case
        QGCToolWidget* tool = widgets.at(i);
        QDockWidget* dock = dynamic_cast<QDockWidget*>(tool->parentWidget());
        if (!dock)
        {
            QSettings settings;
            settings.beginGroup("QGC_MAINWINDOW");

            // Load dock widget location (default is bottom)
            Qt::DockWidgetArea location = static_cast <Qt::DockWidgetArea>(tool->getDockWidgetArea(currentView));

            int view = settings.value(QString("TOOL_PARENT_") + tool->objectName(),-1).toInt();

            settings.endGroup();

            QDockWidget* dock;

            switch (view)
            {
            case VIEW_ENGINEER:
                dock = createDockWidget(engineeringView,tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            case VIEW_FLIGHT:
                dock = createDockWidget(pilotView,tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            case VIEW_SIMULATION:
                dock = createDockWidget(simView,tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            case VIEW_MISSION:
                dock = createDockWidget(plannerView,tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            case VIEW_MAVLINK:
                dock = createDockWidget(mavlinkView,tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            default:
                dock = createDockWidget(centerStack->currentWidget(),tool,tool->getTitle(),tool->objectName(),(VIEW_SECTIONS)view,location);
                break;
            }

            // XXX temporary "fix"
            dock->hide();
        }
    }
}

#ifndef QGC_TOOLBAR_ENABLED
APMToolBar& MainWindow::toolBar()
{
    return *m_apmToolBar;
}
#endif

void MainWindow::buildCommonWidgets()
{
    //TODO:  move protocol outside UI
    connect(LinkManager::instance(),SIGNAL(protocolStatusMessage(QString,QString)),this,SLOT(showCriticalMessage(QString,QString)));

    // Log player
    logPlayer = new QGCMAVLinkLogPlayer(customStatusBar);

    connect(logPlayer,SIGNAL(logFinished()),statusBar(),SLOT(hide()));
    customStatusBar->setLogPlayer(logPlayer);

    // Center widgets
    if (!plannerView)
    {
        plannerView = new SubMainWindow(this);
        plannerView->setObjectName("VIEW_MISSION");
        plannerView->setCentralWidget(new QGCMapTool(this));
        addToCentralStackedWidget(plannerView, VIEW_MISSION, "Maps");
    }

    // pilotView (aka Flight or Mission View)
    if (!pilotView)
    {
        pilotView = new SubMainWindow(this);
        pilotView->setObjectName("VIEW_FLIGHT");
        pilotView->setCentralWidget(new QGCMapTool(this));
        addToCentralStackedWidget(pilotView, VIEW_FLIGHT, "Pilot");
    }

    if (!configView)
    {
        configView = new SubMainWindow(this);
        configView->setObjectName("VIEW_HARDWARE_CONFIG");
        ApmHardwareConfig* aphw = new ApmHardwareConfig(this);
        configView->setCentralWidget(aphw);
        addToCentralStackedWidget(configView,VIEW_HARDWARE_CONFIG, tr("Hardware"));
        connect(ui.actionAdvanced_Mode, SIGNAL(toggled(bool)), aphw, SLOT(advModeChanged(bool)));
    }

    if (!softwareConfigView)
    {
        softwareConfigView = new SubMainWindow(this);
        softwareConfigView->setObjectName("VIEW_SOFTWARE_CONFIG");
        ApmSoftwareConfig* apsw = new ApmSoftwareConfig(this);
        softwareConfigView->setCentralWidget(apsw);
        addToCentralStackedWidget(softwareConfigView, VIEW_SOFTWARE_CONFIG, tr("Software"));
        connect(ui.actionAdvanced_Mode, SIGNAL(toggled(bool)), apsw, SLOT(advModeChanged(bool)));
    }

    AP2DataPlot2D *plot = NULL;
    if (!engineeringView)
    {
        engineeringView = new SubMainWindow(this);
        engineeringView->setObjectName("VIEW_ENGINEER");
        //engineeringView->setCentralWidget(new QGCDataPlot2D(this));
        plot = new AP2DataPlot2D(this);
        connect(logPlayer,SIGNAL(logLoaded()),plot,SLOT(clearGraph()));
        engineeringView->setCentralWidget(plot);

        addToCentralStackedWidget(engineeringView, VIEW_ENGINEER, tr("Logfile Plot"));
    }

    if (!simView)
    {
        simView = new SubMainWindow(this);
        simView->setObjectName("VIEW_SIMULATOR");
        simView->setCentralWidget(new QGCMapTool(this));
        addToCentralStackedWidget(simView, VIEW_SIMULATION, tr("Simulation View"));
    }

    if (!debugOutput)
    {
       debugOutput = DebugOutput::Ptr(new DebugOutput);
       LogWindowSingleton::instance().setDebugOutput(debugOutput);
    }

    // Dock widgets
    QAction* tempAction = ui.menuTools->addAction(tr("Control"));
    tempAction->setCheckable(true);
    connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));

    createDockWidget(simView,new UASControlWidget(this),tr("Control"),"UNMANNED_SYSTEM_CONTROL_DOCKWIDGET",VIEW_SIMULATION,Qt::LeftDockWidgetArea);
    createDockWidget(plannerView,new UASListWidget(this),tr("Unmanned Systems"),"UNMANNED_SYSTEM_LIST_DOCKWIDGET",VIEW_MISSION,Qt::LeftDockWidgetArea);
    createDockWidget(plannerView,new QGCWaypointListMulti(this),tr("Mission Plan"),"WAYPOINT_LIST_DOCKWIDGET",VIEW_MISSION,Qt::BottomDockWidgetArea);

    {   // Widget that shows the elevation changes over a mission.
        QAction* tempAction = ui.menuTools->addAction(tr("Mission Elevation"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "MISSION_ELEVATION_DOCKWIDGET";
    }

    createDockWidget(simView,new QGCWaypointListMulti(this),tr("Mission Plan"),"WAYPOINT_LIST_DOCKWIDGET",VIEW_SIMULATION,Qt::BottomDockWidgetArea);
    createDockWidget(simView,new ParameterInterface(this),tr("Parameters"),"PARAMETER_INTERFACE_DOCKWIDGET",VIEW_SIMULATION,Qt::RightDockWidgetArea);

    {
        QAction* tempAction = ui.menuTools->addAction(tr("Flight Display"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "HEAD_DOWN_DISPLAY_1_DOCKWIDGET";
    }

    // This is required since we disabled the only existing parent window for the MAVLink Inspector
    {
        QAction* tempAction = ui.menuTools->addAction(tr("MAVLink Inspector"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "MAVLINK_INSPECTOR_DOCKWIDGET";
    }

#ifndef PFD_QML
    createDockWidget(simView,new PrimaryFlightDisplay(320,240,this),tr("Primary Flight Display"),
                     "PRIMARY_FLIGHT_DISPLAY_DOCKWIDGET",VIEW_SIMULATION,Qt::RightDockWidgetArea);
    createDockWidget(pilotView,new PrimaryFlightDisplay(320,240,this),tr("Primary Flight Display"),
                     "PRIMARY_FLIGHT_DISPLAY_DOCKWIDGET",VIEW_FLIGHT,Qt::LeftDockWidgetArea);

    { //This is required since we don't show the new PFD in full yet
        QAction* tempAction = ui.menuTools->addAction(tr("Primary Flight Display (2)"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "PRIMARY_FLIGHT_DISPLAY_QML_DOCKWIDGET";
    }
#else
    createDockWidget(simView,new PrimaryFlightDisplayQML(this),tr("Primary Flight Display"),
                     "PRIMARY_FLIGHT_DISPLAY_QML_DOCKWIDGET",VIEW_SIMULATION,Qt::RightDockWidgetArea);
    createDockWidget(pilotView,new PrimaryFlightDisplayQML(this),tr("Primary Flight Display"),
                     "PRIMARY_FLIGHT_DISPLAY_QML_DOCKWIDGET",VIEW_FLIGHT,Qt::LeftDockWidgetArea);

    { // This is required since we don't show the old PFD in any view
        QAction* tempAction = ui.menuTools->addAction(tr("Primary Flight Display (old)"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "PRIMARY_FLIGHT_DISPLAY_DOCKWIDGET";
    }
#endif

    { // Adds the Vibration Monitor Tool
        QAction* tempAction = ui.menuTools->addAction(tr("Vibration Monitor"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "VIBRATION_MONITOR_DOCKWIDGET";
    }

    { // Adds the EKF Monitor Tool
        QAction* tempAction = ui.menuTools->addAction(tr("EKF Monitor"));
        tempAction->setCheckable(true);
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        menuToDockNameMap[tempAction] = "EKF_MONITOR_DOCKWIDGET";
    }

    QGCTabbedInfoView *infoview = new QGCTabbedInfoView(this);
    infoview->addSource(mavlinkDecoder);
    createDockWidget(pilotView,infoview,tr("Info View"),"UAS_INFO_INFOVIEW_DOCKWIDGET",VIEW_FLIGHT,Qt::LeftDockWidgetArea);

    // connect(ui.actionLoad_tlog,SIGNAL(triggered()),this,SLOT(loadTlogMenuClicked()));

    // Custom widgets, added last to all menus and layouts
    buildCustomWidget();



#ifdef QGC_OSG_ENABLE
    if (q3DWidget)
    {
        q3DWidget = Q3DWidgetFactory::get("PIXHAWK", this);
        q3DWidget->setObjectName("VIEW_3DWIDGET");

        addToCentralStackedWidget(q3DWidget, VIEW_3DWIDGET, tr("Local 3D"));
    }
#endif

#if defined(GOOGLE_EARTH_VIEW)/*(defined _MSC_VER) | (defined Q_OS_MAC)*/
    if (!earthWidget)
    {
        earthWidget = new QGCGoogleEarthView(this);
        addToCentralStackedWidget(earthWidget,VIEW_GOOGLEEARTH, tr("Google Earth"));
    }
#endif
}

void MainWindow::addTool(SubMainWindow *parent,VIEW_SECTIONS view,QDockWidget* widget, const QString& title, Qt::DockWidgetArea area)
{
    QList<QAction*> actionlist = ui.menuTools->actions();
    bool found = false;
    QAction *targetAction;
    for (int i=0;i<actionlist.size();i++)
    {
        if (actionlist[i]->text() == title)
        {
            found = true;
            targetAction = actionlist[i];
        }
    }
    if (!found)
    {
        QAction* tempAction = ui.menuTools->addAction(title);
        tempAction->setCheckable(true);
        menuToDockNameMap[tempAction] = widget->objectName();
        if (!centralWidgetToDockWidgetsMap.contains(view))
        {
            centralWidgetToDockWidgetsMap[view] = QMap<QString,QWidget*>();
        }
        centralWidgetToDockWidgetsMap[view][widget->objectName()]= widget;
        connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
        connect(widget, SIGNAL(visibilityChanged(bool)), tempAction, SLOT(setChecked(bool)));
        tempAction->setChecked(widget->isVisible());
    }
    else
    {
        if (!menuToDockNameMap.contains(targetAction))
        {
            menuToDockNameMap[targetAction] = widget->objectName();
        }
        if (!centralWidgetToDockWidgetsMap.contains(view))
        {
            centralWidgetToDockWidgetsMap[view] = QMap<QString,QWidget*>();
        }
        centralWidgetToDockWidgetsMap[view][widget->objectName()]= widget;
        connect(widget, SIGNAL(visibilityChanged(bool)), targetAction, SLOT(setChecked(bool)));
    }
    parent->addDockWidget(area,widget);
}

QDockWidget* MainWindow::createDockWidget(QWidget *parent,QWidget *child,QString title,QString objectname,VIEW_SECTIONS view,Qt::DockWidgetArea area,int minwidth,int minheight)
{
    child->setObjectName(objectname);

    QDockWidget *widget = new QDockWidget(title,this);
    if (!isAdvancedMode)
    {
        if (dockWidgetTitleBarEnabled)
        {
            dockToTitleBarMap[widget] = widget->titleBarWidget();
            QLabel *label = new QLabel(this);
            label->setText(title);
            widget->setTitleBarWidget(label);
            label->installEventFilter(new DockWidgetTitleBarEventFilter());
        }
        else
        {
            dockToTitleBarMap[widget] = widget->titleBarWidget();
            widget->setTitleBarWidget(new QWidget(this));
        }
    }
    else
    {
        QLabel *label = new QLabel(this);
        label->setText(title);
        dockToTitleBarMap[widget] = label;
        label->installEventFilter(new DockWidgetTitleBarEventFilter());
        label->hide();
    }
    widget->setObjectName(child->objectName());
    widget->setWidget(child);
    if (minheight != 0 || minwidth != 0)
    {
        widget->setMinimumHeight(minheight);
        widget->setMinimumWidth(minwidth);
    }
    addTool(qobject_cast<SubMainWindow*>(parent),view,widget,title,area);

    return widget;
}
void MainWindow::loadDockWidget(QString name)
{
    if (centralWidgetToDockWidgetsMap[currentView].contains(name))
    {
        return;
    }
    if (name.startsWith("HIL_CONFIG"))
    {
        //It's a HIL widget.
        showHILConfigurationWidget(UASManager::instance()->getActiveUAS());
    }
    else if (name == "UNMANNED_SYSTEM_CONTROL_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new UASControlWidget(this),tr("Control"),"UNMANNED_SYSTEM_CONTROL_DOCKWIDGET",currentView,Qt::LeftDockWidgetArea);
    }
    else if (name == "UNMANNED_SYSTEM_LIST_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new UASListWidget(this),tr("Unmanned Systems"),"UNMANNED_SYSTEM_LIST_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "WAYPOINT_LIST_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new QGCWaypointListMulti(this),tr("Mission Plan"),"WAYPOINT_LIST_DOCKWIDGET",currentView,Qt::BottomDockWidgetArea);
    }
    else if (name == "MISSION_ELEVATION_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new MissionElevationDisplay(this),tr("Mission Elevation"),"MISSION_ELEVATION_DOCKWIDGET",currentView,Qt::TopDockWidgetArea);
    }
    else if (name == "VIBRATION_MONITOR_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new VibrationMonitor(this),tr("Vibration Monitor"),"VIBRATION_MONITOR_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "EKF_MONITOR_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new EKFMonitor(this),tr("EKF Monitor"),"EKF_MONITOR_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "MAVLINK_INSPECTOR_DOCKWIDGET")
    {
        QGCMAVLinkInspector *widget = new QGCMAVLinkInspector(0,this);
        logPlayer->setMavlinkInspector(widget);
        createDockWidget(centerStack->currentWidget(),widget,tr("MAVLink Inspector"),"MAVLINK_INSPECTOR_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "PARAMETER_INTERFACE_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new ParameterInterface(this),tr("Parameters"),"PARAMETER_INTERFACE_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "UAS_STATUS_DETAILS_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new UASInfoWidget(this),tr("Status Details"),"UAS_STATUS_DETAILS_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "HORIZONTAL_SITUATION_INDICATOR_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new HSIDisplay(this),tr("Horizontal Situation"),"HORIZONTAL_SITUATION_INDICATOR_DOCKWIDGET",currentView,Qt::BottomDockWidgetArea);
    }
    else if (name == "HEAD_DOWN_DISPLAY_1_DOCKWIDGET")
    {
        // FIXME: memory of acceptList will never be freed again
        QStringList* acceptList = new QStringList();
        acceptList->append("-3.3,ATTITUDE.roll,rad,+3.3,s");
        acceptList->append("-3.3,ATTITUDE.pitch,deg,+3.3,s");
        acceptList->append("-3.3,ATTITUDE.yaw,deg,+3.3,s");
        HDDisplay *hddisplay = new HDDisplay(acceptList,"Flight Display",this);
        hddisplay->addSource(mavlinkDecoder);
        createDockWidget(centerStack->currentWidget(),hddisplay,tr("Flight Display"),"HEAD_DOWN_DISPLAY_1_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "HEAD_DOWN_DISPLAY_2_DOCKWIDGET")
    {
        // FIXME: memory of acceptList2 will never be freed again
        QStringList* acceptList2 = new QStringList();
        acceptList2->append("0,RAW_PRESSURE.pres_abs,hPa,65500");
        HDDisplay *hddisplay = new HDDisplay(acceptList2,"Actuator Status",this);
        hddisplay->addSource(mavlinkDecoder);
        createDockWidget(centerStack->currentWidget(),hddisplay,tr("Actuator Status"),"HEAD_DOWN_DISPLAY_2_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "Radio Control")
    {
        QLOG_DEBUG() << "Error loading window:" << name << "Unknown window type";
    }
    else if (name == "PRIMARY_FLIGHT_DISPLAY_DOCKWIDGET")
    {
        // createDockWidget(centerStack->currentWidget(),new HUD(320,240,this),tr("Head Up Display"),"PRIMARY_FLIGHT_DISPLAY_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
        createDockWidget(centerStack->currentWidget(),new PrimaryFlightDisplay(320,240,this),tr("Primary Flight Display"),"HEAD_UP_DISPLAY_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "PRIMARY_FLIGHT_DISPLAY_QML_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new PrimaryFlightDisplayQML(this),tr("Primary Flight Display QML"),"HEAD_UP_DISPLAY_DOCKWIDGET",currentView,Qt::RightDockWidgetArea);
    }
    else if (name == "UAS_INFO_QUICKVIEW_DOCKWIDGET")
    {
        createDockWidget(centerStack->currentWidget(),new UASQuickView(this),tr("Quick View"),"UAS_INFO_QUICKVIEW_DOCKWIDGET",currentView,Qt::LeftDockWidgetArea);
    }
    else
    {
        if (customWidgetNameToFilenameMap.contains(name))
        {
            loadCustomWidget(customWidgetNameToFilenameMap[name],currentView);
        }
        else
        {
            QLOG_DEBUG() << "Error loading window:" << name;
        }
    }
}

void MainWindow::showTool(bool show)
{
    // Called when a menu item is clicked on, regardless of view.

    QAction* act = qobject_cast<QAction *>(sender());
    if (menuToDockNameMap.contains(act))
    {
        QString name = menuToDockNameMap[act];
        if (centralWidgetToDockWidgetsMap.contains(currentView))
        {
            if (centralWidgetToDockWidgetsMap[currentView].contains(name))
            {
                if (show)
                {
                    centralWidgetToDockWidgetsMap[currentView][name]->show();
                }
                else
                {
                    centralWidgetToDockWidgetsMap[currentView][name]->hide();
                }
            }
            else if (show)
            {
                loadDockWidget(name);
            }
        }
    }
}

void MainWindow::addToCentralStackedWidget(QWidget* widget, VIEW_SECTIONS viewSection, const QString& title)
{
    Q_UNUSED(title);
    Q_ASSERT(widget->objectName().length() != 0);

    // Check if this widget already has been added
    if (centerStack->indexOf(widget) == -1)
    {
        centerStack->addWidget(widget);
        centralWidgetToDockWidgetsMap[viewSection] = QMap<QString,QWidget*>();
    }
}


void MainWindow::showCentralWidget()
{
    QAction* act = qobject_cast<QAction *>(sender());
    QWidget* widget = act->data().value<QWidget*>();
    centerStack->setCurrentWidget(widget);
}

void MainWindow::showHILConfigurationWidget(UASInterface* uas)
{
    // Add simulation configuration widget
    UAS* mav = dynamic_cast<UAS*>(uas);

    if (mav && !hilDocks.contains(mav->getUASID()))
    {
        QGCHilConfiguration* hconf = new QGCHilConfiguration(mav, this);
        QString hilDockName = tr("HIL Config %1").arg(uas->getUASName());
        QDockWidget* hilDock = createDockWidget(simView, hconf,hilDockName, hilDockName.toUpper().replace(" ", "_"),VIEW_SIMULATION,Qt::LeftDockWidgetArea);
        hilDocks.insert(mav->getUASID(), hilDock);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isVisible()) storeViewState();
    aboutToCloseFlag = true;
    storeSettings();
    //mavlink->storeSettings();
    UASManager::instance()->storeSettings();
    QMainWindow::closeEvent(event);
}

/**
 * Connect the signals and slots of the common window widgets
 */
void MainWindow::connectCommonWidgets()
{

}

void MainWindow::createCustomWidget()
{

    if (QGCToolWidget::instances()->size() < 2)
    {
        // This is the first widget
        ui.menuTools->addSeparator();
    }
    QGCToolWidget* tool = new QGCToolWidget("Unnamed Tool " + QString::number(ui.menuTools->actions().size()));
    createDockWidget(centerStack->currentWidget(),tool,"Unnamed Tool " + QString::number(ui.menuTools->actions().size()),"UNNAMED_TOOL_" + QString::number(ui.menuTools->actions().size())+"DOCK",currentView,Qt::BottomDockWidgetArea);

    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    settings.setValue(QString("TOOL_PARENT_") + tool->objectName(),currentView);
    settings.endGroup();

}

void MainWindow::loadCustomWidget()
{
    QString widgetFileExtension(".qgw");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Specify Widget File Name"),
                                                    QGC::appDataDirectory(),
                                                    tr("QGroundControl Widget (*%1);;").arg(widgetFileExtension));
    if (fileName != "") loadCustomWidget(fileName);
}
void MainWindow::loadCustomWidget(const QString& fileName, int view)
{
    QGCToolWidget* tool = new QGCToolWidget("", this);
    if (tool->loadSettings(fileName, true))
    {
        QLOG_DEBUG() << "Loading custom tool:" << tool->getTitle() << tool->objectName();
        switch ((VIEW_SECTIONS)view)
        {
        case VIEW_ENGINEER:
            createDockWidget(engineeringView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_FLIGHT:
            createDockWidget(pilotView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_SIMULATION:
            createDockWidget(simView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_MISSION:
            createDockWidget(plannerView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        default:
        {
            // Delete tool, create menu item to tie it to.
            customWidgetNameToFilenameMap[tool->objectName()+"DOCK"] = fileName;
            QAction* tempAction = ui.menuTools->addAction(tool->getTitle());
            menuToDockNameMap[tempAction] = tool->objectName()+"DOCK";
            tempAction->setCheckable(true);
            connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
            tool->deleteLater();
        }
            break;
        }
    }
    else
    {
        return;
    }
}

void MainWindow::loadCustomWidget(const QString& fileName, bool singleinstance)
{
    QGCToolWidget* tool = new QGCToolWidget("", this);
    if (tool->loadSettings(fileName, true) || !singleinstance)
    {
        QLOG_DEBUG() << "Loading custom tool:" << tool->getTitle() << tool->objectName();
        QSettings settings;
        settings.beginGroup("QGC_MAINWINDOW");

        int view = settings.value(QString("TOOL_PARENT_") + tool->objectName(),-1).toInt();
        switch (view)
        {
        case VIEW_ENGINEER:
            createDockWidget(engineeringView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_FLIGHT:
            createDockWidget(pilotView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_SIMULATION:
            createDockWidget(simView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        case VIEW_MISSION:
            createDockWidget(plannerView,tool,tool->getTitle(),tool->objectName()+"DOCK",(VIEW_SECTIONS)view,Qt::LeftDockWidgetArea);
            break;
        default:
        {
            // Delete tool, create menu item to tie it to.
            customWidgetNameToFilenameMap[tool->objectName()+"DOCK"] = fileName;
            QAction* tempAction = ui.menuTools->addAction(tool->getTitle());
            menuToDockNameMap[tempAction] = tool->objectName()+"DOCK";
            tempAction->setCheckable(true);
            connect(tempAction,SIGNAL(triggered(bool)),this, SLOT(showTool(bool)));
            tool->deleteLater();
        }
            break;
        }


        settings.endGroup();
    }
    else
    {
        return;
    }
}

void MainWindow::loadCustomWidgetsFromDefaults(const QString& systemType, const QString& autopilotType)
{
    QString defaultsDir = QGC::shareDirectory() + "/files/" + autopilotType.toLower() + "/widgets/";
    QString platformDir = QGC::shareDirectory() + "/files/" + autopilotType.toLower() + "/" + systemType.toLower() + "/widgets/";

    QDir widgets(defaultsDir);
    QStringList files = widgets.entryList();
    QDir platformWidgets(platformDir);
    files.append(platformWidgets.entryList());

    if (files.count() == 0)
    {
        QLOG_DEBUG() << "No default custom widgets for system " << systemType << "autopilot" << autopilotType << " found";
        QLOG_DEBUG() << "Tried with path: " << defaultsDir;
        showStatusMessage(tr("Did not find any custom widgets in %1").arg(defaultsDir));
    }

    // Load all custom widgets found in the AP folder
    for(int i = 0; i < files.count(); ++i)
    {
        QString file = files[i];
        if (file.endsWith(".qgw"))
        {
            // Will only be loaded if not already a custom widget with
            // the same name is present
            loadCustomWidget(defaultsDir+"/"+file, true);
            showStatusMessage(tr("Loaded custom widget %1").arg(defaultsDir+"/"+file));
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    autoReconnect = settings.value("AUTO_RECONNECT",false).toBool();
    currentStyle = (QGC_MAINWINDOW_STYLE)settings.value("CURRENT_STYLE", QGC_MAINWINDOW_STYLE_OUTDOOR).toInt();
    currentView= static_cast<VIEW_SECTIONS>(settings.value("CURRENT_VIEW", VIEW_FLIGHT).toInt());
    lowPowerMode = settings.value("LOW_POWER_MODE", false).toBool();
    autoProxyMode = settings.value("AUTO_PROXY_MODE", false).toBool();
    dockWidgetTitleBarEnabled = settings.value("DOCK_WIDGET_TITLEBARS", true).toBool();
    isAdvancedMode = settings.value("ADVANCED_MODE", false).toBool();
    enableHeartbeat(settings.value("HEARTBEATS_ENABLED",true).toBool());
    settings.endGroup();
}

void MainWindow::storeSettings()
{
    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    settings.setValue("AUTO_RECONNECT", autoReconnect);
    settings.setValue("CURRENT_STYLE", currentStyle);
    settings.setValue("LOW_POWER_MODE", lowPowerMode);
    settings.setValue("AUTO_PROXY_MODE", autoProxyMode);
    settings.setValue("ADVANCED_MODE", isAdvancedMode);
    settings.setValue("HEARTBEATS_ENABLED",m_heartbeatEnabled);
    settings.endGroup();

    if (!aboutToCloseFlag && isVisible())
    {
        settings.setValue(getWindowGeometryKey(), saveGeometry());
        // Save the last current view in any case
        settings.setValue("CURRENT_VIEW", currentView);
        // Save the current window state, but only if a system is connected (else no real number of widgets would be present))
        if (UASManager::instance()->getUASList().length() > 0) settings.setValue(getWindowStateKey(), saveState(QGC::applicationVersion()));
        // Save the current view only if a UAS is connected
        if (UASManager::instance()->getUASList().length() > 0) settings.setValue("CURRENT_VIEW_WITH_UAS_CONNECTED", currentView);
        // Save the current power mode
    }
    settings.sync();
}

void MainWindow::configureWindowName()
{
    QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
    QString windowname = qApp->applicationName() + " " + qApp->applicationVersion();
    bool prevAddr = false;

    windowname.append(" (" + QHostInfo::localHostName() + ": ");

    for (int i = 0; i < hostAddresses.size(); i++)
    {
        // Exclude loopback IPv4 and all IPv6 addresses
        if (hostAddresses.at(i) != QHostAddress("127.0.0.1") && !hostAddresses.at(i).toString().contains(":"))
        {
            if(prevAddr) windowname.append("/");
            windowname.append(hostAddresses.at(i).toString());
            prevAddr = true;
        }
    }

    windowname.append(")");

    setWindowTitle(windowname);

}

void MainWindow::startVideoCapture()
{
    QString format = "bmp";
    QString initialPath = QGC::appDataDirectory();

    QString screenFileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                          initialPath,
                                                          tr("%1 Files (*.%2);;All Files (*)")
                                                          .arg(format.toUpper())
                                                          .arg(format));
    delete videoTimer;
    videoTimer = new QTimer(this);
}

void MainWindow::stopVideoCapture()
{
    videoTimer->stop();

    // TODO Convert raw images to PNG
}

void MainWindow::saveScreen()
{
    QPixmap window = QPixmap::grabWindow(this->winId());
    QString format = "bmp";

    if (!screenFileName.isEmpty())
    {
        window.save(screenFileName, format.toLatin1());
    }
}
void MainWindow::enableDockWidgetTitleBars(bool enabled)
{
    dockWidgetTitleBarEnabled = enabled;
    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    settings.setValue("DOCK_WIDGET_TITLEBARS",dockWidgetTitleBarEnabled);
    settings.endGroup();
    settings.sync();
    if (!isAdvancedMode)
    {
        if (enabled)
        {
            for (QMap<QDockWidget*,QWidget*>::const_iterator i=dockToTitleBarMap.constBegin();i!=dockToTitleBarMap.constEnd();i++)
            {
                QLabel *label = new QLabel(this);
                label->setText(i.key()->windowTitle());
                i.key()->setTitleBarWidget(label);
                label->installEventFilter(new DockWidgetTitleBarEventFilter());
            }
        }
        else
        {
            for (QMap<QDockWidget*,QWidget*>::const_iterator i=dockToTitleBarMap.constBegin();i!=dockToTitleBarMap.constEnd();i++)
            {
                i.key()->setTitleBarWidget(new QWidget(this));
            }
        }
    }
}

void MainWindow::enableAutoReconnect(bool enabled)
{
    autoReconnect = enabled;
}

void MainWindow::enableAutoProxyMode(bool enabled)
{
    if (enabled)
    {
        QLOG_INFO() << "NETWORK_PROXY:" << "Attempting to enable System Network Proxies";
        QNetworkProxyFactory::setUseSystemConfiguration(true);

        // Check for proxy used for well known external URL
        QNetworkProxyQuery npq(QUrl("http://www.google.com"));
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);

        if (listOfProxies.size() &&
                QNetworkProxy::NoProxy != listOfProxies[0].type())
        {
            QLOG_INFO() << "NETWORK_PROXY:" << "System Proxies in use for external urls";
            autoProxyMode = enabled;
        }
        else
        {
            QLOG_ERROR() << "NETWORK_PROXY:" << "No System Proxies found in environment";
            QNetworkProxyFactory::setUseSystemConfiguration(false);
        }
    }
    else
    {
        QLOG_INFO() << "NETWORK_PROXY:" << "Disabling System Network Proxies";
        QNetworkProxyFactory::setUseSystemConfiguration(false);

        autoProxyMode = enabled;
    }

    // Ensure the checkbox is in-sync with the current value
    emit autoProxyChanged(autoProxyMode);
}

void MainWindow::loadNativeStyle()
{
    loadStyle(QGC_MAINWINDOW_STYLE_NATIVE);
}

void MainWindow::loadIndoorStyle()
{
    loadStyle(QGC_MAINWINDOW_STYLE_INDOOR);
}

void MainWindow::loadOutdoorStyle()
{
    loadStyle(QGC_MAINWINDOW_STYLE_OUTDOOR);
}

void MainWindow::loadStyle(QGC_MAINWINDOW_STYLE style)
{
    switch (style) {
    case QGC_MAINWINDOW_STYLE_NATIVE: {
        // Native mode means setting no style
        // so if we were already in native mode
        // take no action
        // Only if a style was set, remove it.
        if (style != currentStyle) {
            qApp->setStyleSheet("QMainWindow::separator { background: rgb(0, 0, 0); width: 5px; height: 5px;}");
            showInfoMessage(tr("Please restart APM Planner"), tr("Please restart APM Planner to switch to fully native look and feel. Currently you have loaded Qt's plastique style."));
        }
    }
        break;
    case QGC_MAINWINDOW_STYLE_INDOOR:
        qApp->setStyle("plastique");
        styleFileName = ":files/styles/style-indoor.css";
        reloadStylesheet();
        break;
    case QGC_MAINWINDOW_STYLE_OUTDOOR:
        qApp->setStyle("plastique");
        styleFileName = ":files/styles/style-outdoor.css";
        reloadStylesheet();
        break;
    }
    currentStyle = style;
}

void MainWindow::selectStylesheet()
{
    // Let user select style sheet
    QFileDialog *dialog = new QFileDialog(this,tr("Specify stylesheet"), styleFileName, tr("CSS Stylesheet (*.css);;"));
    dialog->setFileMode(QFileDialog::ExistingFile);
    connect(dialog,SIGNAL(accepted()),this,SLOT(selectStylesheetDialogAccepted()));
    dialog->show();
}
void MainWindow::selectStylesheetDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        // No file selected/cancel clicked
        return;
    }
    QString tmpfilename = dialog->selectedFiles().at(0);

    if (!tmpfilename.endsWith(".css"))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(tr("QGroundControl did lot load a new style"));
        msgBox.setInformativeText(tr("No suitable .css file selected. Please select a valid .css file."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    styleFileName = tmpfilename;

    // Load style sheet
    reloadStylesheet();
}

void MainWindow::reloadStylesheet()
{
    // Load style sheet
    QScopedPointer<QFile> styleSheet(new QFile(styleFileName));
    if (!styleSheet->exists())
    {
        styleSheet.reset(new QFile(":files/styles/style-outdoor.css"));
    }
    if (styleSheet->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString style = QString(styleSheet->readAll());
        style.replace("ICONDIR", QGC::shareDirectory() + "/files/styles/");
        qApp->setStyleSheet(style);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(tr("QGroundControl did lot load a new style"));
        msgBox.setInformativeText(tr("Stylesheet file %1 was not readable").arg(styleFileName));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

/**
 * The status message will be overwritten if a new message is posted to this function
 *
 * @param status message text
 * @param timeout how long the status should be displayed
 */
void MainWindow::showStatusMessage(const QString& status, int timeout)
{
    statusBar()->showMessage(status, timeout);
}

/**
 * The status message will be overwritten if a new message is posted to this function.
 * it will be automatically hidden after 5 seconds.
 *
 * @param status message text
 */
void MainWindow::showStatusMessage(const QString& status)
{
    statusBar()->showMessage(status, 20000);
}

void MainWindow::showCriticalMessage(const QString& title, const QString& message)
{
    qDebug() << "Critical message:" << title << message;
}

void MainWindow::showInfoMessage(const QString& title, const QString& message)
{
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(title);
    msgBox.setInformativeText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

/**
* @brief Create all actions associated to the main window
*
**/
void MainWindow::connectCommonActions()
{
    // Bind together the perspective actions
    QActionGroup* perspectives = new QActionGroup(ui.menuPerspectives);
    perspectives->addAction(ui.actionEngineersView);
    perspectives->addAction(ui.actionMavlinkView);
    perspectives->addAction(ui.actionFlightView);
    perspectives->addAction(ui.actionSimulation_View);
    perspectives->addAction(ui.actionMissionView);
    // perspectives->addAction(ui.actionConfiguration_2);
    perspectives->addAction(ui.actionHardwareConfig);
    perspectives->addAction(ui.actionSoftwareConfig);
    // perspectives->addAction(ui.actionFirmwareUpdateView);
    perspectives->addAction(ui.actionTerminalView);
    // perspectives->addAction(ui.actionUnconnectedView);
    perspectives->setExclusive(true);

    // Mark the right one as selected
    if (currentView == VIEW_ENGINEER)
    {
        ui.actionEngineersView->setChecked(true);
        ui.actionEngineersView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_MAVLINK)
    {
        ui.actionMavlinkView->setChecked(true);
        ui.actionMavlinkView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_FLIGHT)
    {
        ui.actionFlightView->setChecked(true);
        ui.actionFlightView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_SIMULATION)
    {
        ui.actionSimulation_View->setChecked(true);
        ui.actionSimulation_View->activate(QAction::Trigger);
    }
    if (currentView == VIEW_MISSION)
    {
        ui.actionMissionView->setChecked(true);
        ui.actionMissionView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_HARDWARE_CONFIG)
    {
        ui.actionHardwareConfig->setChecked(true);
        ui.actionHardwareConfig->activate(QAction::Trigger);
    }
    if (currentView == VIEW_SOFTWARE_CONFIG)
    {
        ui.actionSoftwareConfig->setChecked(true);
        ui.actionSoftwareConfig->activate(QAction::Trigger);
    }
    if (currentView == VIEW_FIRMWAREUPDATE)
    {
        ui.actionFirmwareUpdateView->setChecked(true);
        ui.actionFirmwareUpdateView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_TERMINAL)
    {
        ui.actionTerminalView->setChecked(true);
        ui.actionTerminalView->activate(QAction::Trigger);
    }
    if (currentView == VIEW_UNCONNECTED)
    {
        ui.actionUnconnectedView->setChecked(true);
        ui.actionUnconnectedView->activate(QAction::Trigger);
    }

    // The UAS actions are not enabled without connection to system
    ui.actionLiftoff->setEnabled(false);
    ui.actionLand->setEnabled(false);
    ui.actionEmergency_Kill->setEnabled(false);
    ui.actionEmergency_Land->setEnabled(false);
    ui.actionShutdownMAV->setEnabled(false);

    // About
    connect(ui.actionAbout_APM_Planner_2_0, SIGNAL(triggered()), this, SLOT(showAbout()));

    // Check for updates
    connect(ui.actionCheck_For_Updates, SIGNAL(triggered()), &m_autoUpdateCheck, SLOT(forcedAutoUpdateCheck()));

    // Connect actions from ui
    ui.actionSerial->setData(LinkInterface::SERIAL_LINK);
    ui.actionTCP->setData(LinkInterface::TCP_LINK);
    ui.actionUDP->setData(LinkInterface::UDP_LINK);
    ui.actionUDPClient->setData(LinkInterface::UDP_CLIENT_LINK);
    connect(ui.actionSerial,SIGNAL(triggered()),this,SLOT(addLink()));
    connect(ui.actionTCP,SIGNAL(triggered()),this,SLOT(addLink()));
    connect(ui.actionUDP,SIGNAL(triggered()),this,SLOT(addLink()));
    connect(ui.actionUDPClient,SIGNAL(triggered()),this,SLOT(addLink()));
    connect(ui.actionAdvanced_Mode,SIGNAL(triggered(bool)),this,SLOT(setAdvancedMode(bool)));

    // Connect internal actions
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(UASCreated(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));

    // Unmanned System controls
    connect(ui.actionLiftoff, SIGNAL(triggered()), UASManager::instance(), SLOT(launchActiveUAS()));
    connect(ui.actionLand, SIGNAL(triggered()), UASManager::instance(), SLOT(returnActiveUAS()));
    connect(ui.actionEmergency_Land, SIGNAL(triggered()), UASManager::instance(), SLOT(stopActiveUAS()));
    connect(ui.actionEmergency_Kill, SIGNAL(triggered()), UASManager::instance(), SLOT(killActiveUAS()));
    connect(ui.actionShutdownMAV, SIGNAL(triggered()), UASManager::instance(), SLOT(shutdownActiveUAS()));
    connect(ui.actionConfiguration, SIGNAL(triggered()), UASManager::instance(), SLOT(configureActiveUAS()));

    // Views actions
    connect(ui.actionFlightView, SIGNAL(triggered()), this, SLOT(loadPilotView()));
    connect(ui.actionSimulation_View, SIGNAL(triggered()), this, SLOT(loadSimulationView()));
    connect(ui.actionEngineersView, SIGNAL(triggered()), this, SLOT(loadEngineerView()));
    connect(ui.actionMissionView, SIGNAL(triggered()), this, SLOT(loadOperatorView()));
    connect(ui.actionUnconnectedView, SIGNAL(triggered()), this, SLOT(loadUnconnectedView()));
    connect(ui.actionHardwareConfig,SIGNAL(triggered()),this,SLOT(loadHardwareConfigView()));
    connect(ui.actionSoftwareConfig,SIGNAL(triggered()),this,SLOT(loadSoftwareConfigView()));
    connect(ui.actionTerminalView,SIGNAL(triggered()),this,SLOT(loadTerminalView()));

    connect(ui.actionFirmwareUpdateView, SIGNAL(triggered()), this, SLOT(loadFirmwareUpdateView()));
    connect(ui.actionMavlinkView, SIGNAL(triggered()), this, SLOT(loadMAVLinkView()));

    connect(ui.actionReloadStylesheet, SIGNAL(triggered()), this, SLOT(reloadStylesheet()));
    connect(ui.actionSelectStylesheet, SIGNAL(triggered()), this, SLOT(selectStylesheet()));

    // Help Actions
    connect(ui.actionOnline_Documentation, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(ui.actionDeveloper_Credits, SIGNAL(triggered()), this, SLOT(showCredits()));
    connect(ui.actionProject_Roadmap_2, SIGNAL(triggered()), this, SLOT(showRoadMap()));

    // Custom widget actions
    connect(ui.actionNewCustomWidget, SIGNAL(triggered()), this, SLOT(createCustomWidget()));
    connect(ui.actionLoadCustomWidgetFile, SIGNAL(triggered()), this, SLOT(loadCustomWidget()));

    // Audio output
    ui.actionMuteAudioOutput->setChecked(GAudioOutput::instance()->isMuted());
    connect(GAudioOutput::instance(), SIGNAL(mutedChanged(bool)), ui.actionMuteAudioOutput, SLOT(setChecked(bool)));
    connect(ui.actionMuteAudioOutput, SIGNAL(triggered(bool)), GAudioOutput::instance(), SLOT(mute(bool)));

    // User interaction
    // NOTE: Joystick thread is not started and
    // configuration widget is not instantiated
    // unless it is actually used
    // so no ressources spend on this.

    // Configuration
    // Joystick
    connect(ui.actionJoystickSettings, SIGNAL(triggered()), this, SLOT(configure()));
    // Application Settings
    connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()));

    if (isAdvancedMode)
    {
        ui.menuPerspectives->menuAction()->setVisible(true);
        ui.menuTools->menuAction()->setVisible(true);
        ui.menuNetwork->menuAction()->setVisible(true);
    }
    else
    {
        ui.menuPerspectives->menuAction()->setVisible(false);
        ui.menuTools->menuAction()->setVisible(false);
        ui.menuNetwork->menuAction()->setVisible(false);
    }

    connect(ui.actionDebug_Console,SIGNAL(triggered()),debugOutput.data(),SLOT(show()));
    connect(ui.actionSimulate, SIGNAL(triggered(bool)), this, SLOT(simulateLink(bool)));

    // Disable simulation view until we ensure it's operational.
    ui.actionSimulate->setVisible(false);
    ui.actionSimulationView->setVisible(false);
}

void MainWindow::showHelp()
{
    if(!QDesktopServices::openUrl(QUrl("http://qgroundcontrol.org/users/start")))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Could not open help in browser");
        msgBox.setInformativeText("To get to the online help, please open http://qgroundcontrol.org/user_guide in a browser.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::showCredits()
{
    if(!QDesktopServices::openUrl(QUrl("http://qgroundcontrol.org/credits")))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Could not open credits in browser");
        msgBox.setInformativeText("To get to the online help, please open http://qgroundcontrol.org/credits in a browser.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::showRoadMap()
{
    if(!QDesktopServices::openUrl(QUrl("http://qgroundcontrol.org/dev/roadmap")))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Could not open roadmap in browser");
        msgBox.setInformativeText("To get to the online help, please open http://qgroundcontrol.org/roadmap in a browser.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::configure()
{
    if (!joystickWidget)
    {
        if (!joystick->isRunning())
        {
            joystick->start();
        }
        joystickWidget = new JoystickWidget(joystick);
    }
    joystickWidget->show();
}

void MainWindow::showSettings()
{
    QGCSettingsWidget* settings = new QGCSettingsWidget(this);
    settings->show();
}



bool MainWindow::configLink(int linkid)
{
    // Go searching for this link's configuration window
    QList<QAction*> actions = ui.menuNetwork->actions();

    bool found(false);

    foreach (QAction* action, actions)
    {
        if (action->data().toInt() == linkid)
        {
            found = true;
            action->trigger(); // Show the Link Config Dialog
        }
    }

    return found;
}
void MainWindow::addLink()
{
    QAction *send = qobject_cast<QAction*>(sender());
    if (!send)
    {
        return;
    }
    int newid = 0;
    if (send->data() == LinkInterface::SERIAL_LINK)
    {
        newid = LinkManagerFactory::addSerialConnection();
    }
    else if (send->data() == LinkInterface::TCP_LINK)
    {
        newid = LinkManagerFactory::addTcpConnection(QHostAddress::LocalHost, "", 5760, false);
    }
    else if (send->data() == LinkInterface::UDP_LINK)
    {
        newid = LinkManagerFactory::addUdpConnection(QHostAddress::LocalHost,14550);
    }
    else if (send->data() == LinkInterface::UDP_CLIENT_LINK)
    {
        newid = LinkManagerFactory::addUdpClientConnection(QHostAddress("192.168.4.1"),14550);
    }
    addLink(newid);
    for (int i=0;i<ui.menuNetwork->actions().size();i++)
    {
        if (ui.menuNetwork->actions().at(i)->data().toInt() == newid)
        {
            // Link already exists!
            ui.menuNetwork->actions().at(i)->trigger();
            return;
        }
    }
}

void MainWindow::addLink(int linkid)
{
    for (int i=0;i<ui.menuNetwork->actions().size();i++)
    {
        if (ui.menuNetwork->actions().at(i)->data().toInt() == linkid)
        {
            // Link already exists!
            return;
        }
    }
    CommConfigurationWindow* commWidget = new CommConfigurationWindow(linkid, 0, this);
    commsWidgetList.append(commWidget);
    connect(commWidget,SIGNAL(destroyed(QObject*)),this,SLOT(commsWidgetDestroyed(QObject*)));
    QAction* action = commWidget->getAction();
    action->setData(linkid);
    ui.menuNetwork->addAction(action);
}

void MainWindow::linkError(int linkid,QString errorstring)
{
    Q_UNUSED(linkid)

    QWidget* parent = QApplication::activeWindow();
    if (!parent) {
        parent = this;
    }
    QMessageBox::information(parent,"Link Error",errorstring);
}

void MainWindow::simulateLink(bool simulate) {
    if (!simulationLink.isNull())
        simulationLink = new MAVLinkSimulationLink(":/demo-log.txt");
    simulationLink->connectLink(simulate);
}

void MainWindow::commsWidgetDestroyed(QObject *obj)
{
    if (commsWidgetList.contains(obj))
    {
        commsWidgetList.removeOne(obj);
    }
}

void MainWindow::setActiveUAS(UASInterface* uas)
{
    Q_UNUSED(uas);
    if (settings.contains(getWindowStateKey()))
    {
        SubMainWindow *win = qobject_cast<SubMainWindow*>(centerStack->currentWidget());
        win->restoreState(settings.value(getWindowStateKey()).toByteArray(), QGC::applicationVersion());
    }

}

void MainWindow::UASSpecsChanged(int uas)
{
    UASInterface* activeUAS = UASManager::instance()->getActiveUAS();
    if (activeUAS)
    {
        if (activeUAS->getUASID() == uas)
        {
        }
    }
}

void MainWindow::UASCreated(UASInterface* uas)
{

    // The pilot, operator and engineer views were not available on startup, enable them now
    ui.actionFlightView->setEnabled(true);
    ui.actionMissionView->setEnabled(true);
    ui.actionEngineersView->setEnabled(true);
    // The UAS actions are not enabled without connection to system
    ui.actionLiftoff->setEnabled(true);
    ui.actionLand->setEnabled(true);
    ui.actionEmergency_Kill->setEnabled(true);
    ui.actionEmergency_Land->setEnabled(true);
    ui.actionShutdownMAV->setEnabled(true);

    QIcon icon;
    // Set matching icon
    switch (uas->getSystemType())
    {
    case MAV_TYPE_GENERIC:
        icon = QIcon(":files/images/mavs/generic.svg");
        break;
    case MAV_TYPE_FIXED_WING:
        icon = QIcon(":files/images/mavs/fixed-wing.svg");
        break;
    case MAV_TYPE_QUADROTOR:
        icon = QIcon(":files/images/mavs/quadrotor.svg");
        break;
    case MAV_TYPE_COAXIAL:
        icon = QIcon(":files/images/mavs/coaxial.svg");
        break;
    case MAV_TYPE_HELICOPTER:
        icon = QIcon(":files/images/mavs/helicopter.svg");
        break;
    case MAV_TYPE_ANTENNA_TRACKER:
        icon = QIcon(":files/images/mavs/antenna-tracker.svg");
        break;
    case MAV_TYPE_GCS:
        icon = QIcon(":files/images/mavs/groundstation.svg");
        break;
    case MAV_TYPE_AIRSHIP:
        icon = QIcon(":files/images/mavs/airship.svg");
        break;
    case MAV_TYPE_FREE_BALLOON:
        icon = QIcon(":files/images/mavs/free-balloon.svg");
        break;
    case MAV_TYPE_ROCKET:
        icon = QIcon(":files/images/mavs/rocket.svg");
        break;
    case MAV_TYPE_GROUND_ROVER:
        icon = QIcon(":files/images/mavs/ground-rover.svg");
        break;
    case MAV_TYPE_SURFACE_BOAT:
        icon = QIcon(":files/images/mavs/surface-boat.svg");
        break;
    case MAV_TYPE_SUBMARINE:
        icon = QIcon(":files/images/mavs/submarine.svg");
        break;
    case MAV_TYPE_HEXAROTOR:
        icon = QIcon(":files/images/mavs/hexarotor.svg");
        break;
    case MAV_TYPE_OCTOROTOR:
        icon = QIcon(":files/images/mavs/octorotor.svg");
        break;
    case MAV_TYPE_TRICOPTER:
        icon = QIcon(":files/images/mavs/tricopter.svg");
        break;
    case MAV_TYPE_FLAPPING_WING:
        icon = QIcon(":files/images/mavs/flapping-wing.svg");
        break;
    case MAV_TYPE_KITE:
        icon = QIcon(":files/images/mavs/kite.svg");
        break;
    default:
        icon = QIcon(":files/images/mavs/unknown.svg");
        break;
    }

    connect(uas, SIGNAL(systemSpecsChanged(int)), this, SLOT(UASSpecsChanged(int)));

    // HIL
    showHILConfigurationWidget(uas);


    // Load default custom widgets for this autopilot type
    loadCustomWidgetsFromDefaults(uas->getSystemTypeName(), uas->getAutopilotTypeName());


    if (uas->getAutopilotType() == MAV_AUTOPILOT_PX4)
    {
        // Dock widgets
        if (!detectionDockWidget)
        {
            detectionDockWidget = new QDockWidget(tr("Object Recognition"), this);
            detectionDockWidget->setWidget( new ObjectDetectionView("/files/images/patterns", this) );
            detectionDockWidget->setObjectName("OBJECT_DETECTION_DOCK_WIDGET");
        }

        if (!watchdogControlDockWidget)
        {
            watchdogControlDockWidget = new QDockWidget(tr("Process Control"), this);
            watchdogControlDockWidget->setWidget( new WatchdogControl(this) );
            watchdogControlDockWidget->setObjectName("WATCHDOG_CONTROL_DOCKWIDGET");
        }
    }

    // Change the view only if this is the first UAS

    // If this is the first connected UAS, it is both created as well as
    // the currently active UAS
    if (UASManager::instance()->getUASList().size() == 1)
    {
        // Load last view if setting is present
        if (settings.contains("CURRENT_VIEW_WITH_UAS_CONNECTED"))
        {

        }
    }

    // Reload view state in case new widgets were added
    loadViewState();
}

void MainWindow::UASDeleted(UASInterface* uas)
{
    Q_UNUSED(uas);
    if (UASManager::instance()->getUASList().count() == 0)
    {
    }
}

/**
 * Stores the current view state
 */
void MainWindow::storeViewState()
{
    if (!aboutToCloseFlag)
    {
        // Save current state
        SubMainWindow *win = qobject_cast<SubMainWindow*>(centerStack->currentWidget());
        QList<QDockWidget*> widgets = win->findChildren<QDockWidget*>();
        QString widgetnames = "";
        for (int i=0;i<widgets.size();i++)
        {
            widgetnames += widgets[i]->objectName() + ",";
        }
        widgetnames = widgetnames.mid(0,widgetnames.length()-1);

        settings.setValue(getWindowStateKey() + "WIDGETS",widgetnames);
        settings.setValue(getWindowStateKey(), win->saveState(QGC::applicationVersion()));
        settings.setValue(getWindowStateKey()+"CENTER_WIDGET", centerStack->currentIndex());
        // Although we want save the state of the window, we do not want to change the top-leve state (minimized, maximized, etc)
        // therefore this state is stored here and restored after applying the rest of the settings in the new
        // perspective.
        windowStateVal = this->windowState();
        settings.setValue(getWindowGeometryKey(), saveGeometry());
    }
}

void MainWindow::loadViewState()
{
    // Restore center stack state
    int index = settings.value(getWindowStateKey()+"CENTER_WIDGET", -1).toInt();
    // The offline plot view is usually the consequence of a logging run, always show the realtime view first
    if (centerStack->indexOf(engineeringView) == index)
    {
        // Rewrite to realtime plot
        // index = centerStack->indexOf(linechartWidget);
    }

    if (index != -1)
    {
        centerStack->setCurrentIndex(index);
    }
    else
    {
        // Hide custom widgets
        if (detectionDockWidget) detectionDockWidget->hide();
        if (watchdogControlDockWidget) watchdogControlDockWidget->hide();

        // Load defaults
        switch (currentView)
        {
        case VIEW_HARDWARE_CONFIG:
            centerStack->setCurrentWidget(configView);
            break;
        case VIEW_SOFTWARE_CONFIG:
            centerStack->setCurrentWidget(softwareConfigView);
            break;
        case VIEW_ENGINEER:
            centerStack->setCurrentWidget(engineeringView);
            break;
        case VIEW_FLIGHT:
            centerStack->setCurrentWidget(pilotView);
            break;
        case VIEW_MAVLINK:
            centerStack->setCurrentWidget(mavlinkView);
            break;
        case VIEW_FIRMWAREUPDATE:
            centerStack->setCurrentWidget(firmwareUpdateWidget);
            break;
        case VIEW_MISSION:
            centerStack->setCurrentWidget(plannerView);
            break;

        case VIEW_SIMULATION:
            centerStack->setCurrentWidget(simView);
            break;

        case VIEW_TERMINAL:
            centerStack->setCurrentWidget(terminalView);
            break;

        case VIEW_UNCONNECTED:
        case VIEW_FULL:
        default:
            if (controlDockWidget)
            {
                controlDockWidget->hide();
            }
            if (listDockWidget)
            {
                listDockWidget->show();
            }
            break;
        }
    }

    // Restore the widget positions and size
    if (settings.contains(getWindowStateKey() + "WIDGETS"))
    {
        QString widgetstr = settings.value(getWindowStateKey() + "WIDGETS").toString();
        QStringList split = widgetstr.split(",");
        foreach (QString widgetname,split)
        {
            if (widgetname != "")
            {
                QLOG_DEBUG() << "Loading widget:" << widgetname;
                loadDockWidget(widgetname);
            }
        }
    }
    if (settings.contains(getWindowStateKey()))
    {
        SubMainWindow *win = qobject_cast<SubMainWindow*>(centerStack->currentWidget());
        win->restoreState(settings.value(getWindowStateKey()).toByteArray(), QGC::applicationVersion());
    }
}
void MainWindow::setAdvancedMode(bool mode)
{
    isAdvancedMode = mode;
    ui.actionAdvanced_Mode->setChecked(mode);
    ui.menuPerspectives->menuAction()->setVisible(mode);
    ui.menuTools->menuAction()->setVisible(mode);
    ui.menuNetwork->menuAction()->setVisible(mode);

    for (QMap<QDockWidget*,QWidget*>::const_iterator i=dockToTitleBarMap.constBegin();
         i!=dockToTitleBarMap.constEnd();i++)
    {
        QWidget *widget = i.key()->titleBarWidget();
        i.key()->setTitleBarWidget(i.value());
        dockToTitleBarMap[i.key()] = widget;
    }
}

void MainWindow::loadEngineerView()
{
    if (currentView != VIEW_ENGINEER)
    {
        storeViewState();
        currentView = VIEW_ENGINEER;
        ui.actionEngineersView->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadOperatorView()
{
    if (currentView != VIEW_MISSION)
    {
        storeViewState();
        currentView = VIEW_MISSION;
        ui.actionMissionView->setChecked(true);
        loadViewState();
    }
}
void MainWindow::loadHardwareConfigView()
{
    if (currentView != VIEW_HARDWARE_CONFIG)
    {
        storeViewState();
        currentView = VIEW_HARDWARE_CONFIG;
        ui.actionHardwareConfig->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadSoftwareConfigView()
{
    if (currentView != VIEW_SOFTWARE_CONFIG)
    {
        storeViewState();
        currentView = VIEW_SOFTWARE_CONFIG;
        ui.actionSoftwareConfig->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadTerminalView()
{
    if (currentView != VIEW_TERMINAL)
    {
        storeViewState();
        currentView = VIEW_TERMINAL;
        ui.actionTerminalView->setChecked(true);
        loadViewState();
    }
}


void MainWindow::loadUnconnectedView()
{
    if (currentView != VIEW_UNCONNECTED)
    {
        storeViewState();
        currentView = VIEW_UNCONNECTED;
        ui.actionUnconnectedView->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadPilotView()
{
    if (currentView != VIEW_FLIGHT)
    {
        storeViewState();
        currentView = VIEW_FLIGHT;
        ui.actionFlightView->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadSimulationView()
{
    if (currentView != VIEW_SIMULATION)
    {
        storeViewState();
        currentView = VIEW_SIMULATION;
        ui.actionSimulation_View->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadMAVLinkView()
{
    if (currentView != VIEW_MAVLINK)
    {
        storeViewState();
        currentView = VIEW_MAVLINK;
        ui.actionMavlinkView->setChecked(true);
        loadViewState();
    }
}

void MainWindow::loadFirmwareUpdateView()
{
    if (currentView != VIEW_FIRMWAREUPDATE)
    {
        storeViewState();
        currentView = VIEW_FIRMWAREUPDATE;
        ui.actionFirmwareUpdateView->setChecked(true);
        loadViewState();
    }
}

QList<QAction*> MainWindow::listLinkMenuActions(void)
{
    return ui.menuNetwork->actions();
}

#ifdef MOUSE_ENABLED_LINUX
bool MainWindow::x11Event(XEvent *event)
{
    emit x11EventOccured(event);
    return false;
}
#endif // MOUSE_ENABLED_LINUX

void MainWindow::showAbout()
{
    AboutDialog* dialog = new AboutDialog(this);
    dialog->exec();
    dialog->hide();
    delete dialog;
    dialog = NULL;
}

void MainWindow::showAutoUpdateDownloadDialog(QString version, QString releaseType, QString url, QString name)
{
    QLOG_DEBUG() << "Update Available! Show Update Dialog";
    QLOG_DEBUG() << "Ver:" << version << "type:" << releaseType;

    m_dialog = new AutoUpdateDialog(version, name, url, this);
    connect(m_dialog, SIGNAL(autoUpdateCancelled(QString)), this, SLOT(autoUpdateCancelled(QString)));
    m_dialog->show();
}

void MainWindow::autoUpdateCancelled(QString version)
{
    QLOG_DEBUG() << "autoUpdateCancelled";
    m_autoUpdateCheck.setSkipVersion(version);

    delete m_dialog;
    m_dialog = NULL;
}

void MainWindow::showNoUpdateAvailDialog()
{
    QMessageBox::information(this,"Update Check", "No new update available!",QMessageBox::Ok);
}
void MainWindow::enableHeartbeat(bool enabled)
{
    if (m_heartbeatEnabled != enabled)
    {
        m_heartbeatEnabled = enabled;
        for (int i=0;i<UASManager::instance()->getUASList().size();i++)
        {
            UASManager::instance()->getUASList().at(i)->setHeartbeatEnabled(enabled);
        }
        storeSettings();
    }
}

void MainWindow::showTerminalConsole()
{
    if(m_terminalDialog == NULL){
        m_terminalDialog = new QDialog(NULL);
        TerminalConsole *terminalConsole = new TerminalConsole(this);
        QVBoxLayout* vLayout = new QVBoxLayout(m_terminalDialog);
        vLayout->setMargin(0);
        vLayout->addWidget(terminalConsole);
        m_terminalDialog->resize(640,325);
        m_terminalDialog->show();
        connect(m_terminalDialog, SIGNAL(finished(int)), this, SLOT(closeTerminalConsole()));
    }

    if (m_terminalDialog){
        m_terminalDialog->raise();
    }
}

void MainWindow::closeTerminalConsole()
{
    if (m_terminalDialog){
        m_terminalDialog->close();
        m_terminalDialog->deleteLater();
        m_terminalDialog = NULL;
    }
}
