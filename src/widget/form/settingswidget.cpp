/*
    Copyright © 2014-2015 by The qTox Project

    This file is part of qTox, a Qt-based graphical interface for Tox.

    qTox is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qTox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qTox.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settingswidget.h"
#include "src/widget/widget.h"
#include "src/video/camerasource.h"
#include "src/widget/form/settings/generalform.h"
#include "src/widget/form/settings/privacyform.h"
#include "src/widget/form/settings/avform.h"
#include "src/widget/form/settings/advancedform.h"
#include "src/widget/form/settings/aboutform.h"
#include "src/widget/translator.h"
#include "src/widget/contentlayout.h"
#include <QTabWidget>
#include <QLabel>
#include <QWindow>

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent, Qt::Window)
{
    body = new QWidget();
    QVBoxLayout* bodyLayout = new QVBoxLayout();
    body->setLayout(bodyLayout);

    head = new QWidget(this);
    QHBoxLayout* headLayout = new QHBoxLayout();
    head->setLayout(headLayout);

    imgLabel = new QLabel();
    headLayout->addWidget(imgLabel);

    nameLabel = new QLabel();
    QFont bold;
    bold.setBold(true);
    nameLabel->setFont(bold);
    headLayout->addWidget(nameLabel);
    headLayout->addStretch(1);

    settingsWidgets = new QTabWidget(this);
    settingsWidgets->setTabPosition(QTabWidget::North);

    bodyLayout->addWidget(settingsWidgets);

    GeneralForm* gfrm = new GeneralForm(this);
    PrivacyForm* pfrm = new PrivacyForm;
    AVForm* avfrm = new AVForm;
    AdvancedForm *expfrm = new AdvancedForm;
    AboutForm *abtfrm = new AboutForm;

    cfgForms = {{ gfrm, pfrm, avfrm, expfrm, abtfrm }};
    for (GenericForm* cfgForm : cfgForms)
        settingsWidgets->addTab(cfgForm, cfgForm->getFormIcon(), cfgForm->getFormName());

    connect(settingsWidgets, &QTabWidget::currentChanged, this, &SettingsWidget::onTabChanged);

    Translator::registerHandler(std::bind(&SettingsWidget::retranslateUi, this), this);
}

SettingsWidget::~SettingsWidget()
{
    Translator::unregister(this);
}

void SettingsWidget::setBodyHeadStyle(QString style)
{
    head->setStyle(QStyleFactory::create(style));
    body->setStyle(QStyleFactory::create(style));
}

void SettingsWidget::showAbout()
{
    onTabChanged(settingsWidgets->count() - 1);
}

bool SettingsWidget::isShown() const
{
    if (body->isVisible())
    {
        body->window()->windowHandle()->alert(0);
        return true;
    }

    return false;
}

void SettingsWidget::show(ContentLayout* contentLayout)
{
    contentLayout->mainContent->layout()->addWidget(body);
    contentLayout->mainHead->layout()->addWidget(head);
    body->show();
    head->show();
    onTabChanged(settingsWidgets->currentIndex());
}

void SettingsWidget::onTabChanged(int index)
{
    this->settingsWidgets->setCurrentIndex(index);
    GenericForm* currentWidget = static_cast<GenericForm*>(this->settingsWidgets->widget(index));
    nameLabel->setText(currentWidget->getFormName());
    imgLabel->setPixmap(currentWidget->getFormIcon().scaledToHeight(40, Qt::SmoothTransformation));
}

void SettingsWidget::retranslateUi()
{
    GenericForm* currentWidget = static_cast<GenericForm*>(settingsWidgets->currentWidget());
    nameLabel->setText(currentWidget->getFormName());
    for (size_t i=0; i<cfgForms.size(); i++)
        settingsWidgets->setTabText(i, cfgForms[i]->getFormName());
}
