/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file
 *   @brief DroneshareUpload helper
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "QsLog.h"
#include "LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    setUpGUI();
    setWindowTitle( tr("User Login") );
    setModal( true );
}

void LoginDialog::setUpGUI(){
    // set up the layout
    QGridLayout* formGridLayout = new QGridLayout( this );

    // initialize the username combo box so that it is editable
    comboUsername = new QComboBox( this );
    comboUsername->setEditable( true );
    // initialize the password field so that it does not echo
    // characters
    editPassword = new QLineEdit( this );
    editPassword->setEchoMode( QLineEdit::Password );

    // initialize the labels
    labelUsername = new QLabel( this );
    labelPassword = new QLabel( this );
    labelUsername->setText( tr( "Username" ) );
    labelUsername->setBuddy( comboUsername );
    labelPassword->setText( tr( "Password" ) );
    labelPassword->setBuddy( editPassword );

    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL(clicked()),
             this,
             SLOT(close())
             );

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL(clicked()),
             this,
             SLOT(slotAcceptLogin()) );

    // place components into the dialog
    formGridLayout->addWidget( labelUsername, 0, 0 );
    formGridLayout->addWidget( comboUsername, 0, 1 );
    formGridLayout->addWidget( labelPassword, 1, 0 );
    formGridLayout->addWidget( editPassword, 1, 1 );
    formGridLayout->addWidget( buttons, 2, 0, 1, 2 );

    setLayout( formGridLayout );
}

void LoginDialog::setUsername(const QString &username){
    bool found = false;
    for( int i = 0; i < comboUsername->count() && ! found ; i++ )
        if( comboUsername->itemText( i ) == username  ){
            comboUsername->setCurrentIndex( i );
            found = true;
        }

    if( ! found ){
        int index = comboUsername->count();
        qDebug() << "Select username " << index;
        comboUsername->addItem( username );

        comboUsername->setCurrentIndex( index );
    }

    // place the focus on the password field
    editPassword->setFocus();
}

void LoginDialog::setPassword(const QString &password){
    editPassword->setText( password );
}

void LoginDialog::slotAcceptLogin(){
    QString username = comboUsername->currentText();
    QString password = editPassword->text();
    int     index    = comboUsername->currentIndex();

    emit acceptLogin( username,  // current username
                      password,  // current password
                      index      // index in the username list
                      );

    // close this dialog
    close();
}

void LoginDialog::setUsernamesList(const QStringList &usernames){
    comboUsername->addItems( usernames );
}
