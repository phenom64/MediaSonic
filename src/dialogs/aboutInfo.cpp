/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "aboutInfo.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

AboutInfo::AboutInfo(QWidget *parent) :
    QDialog(parent)
{
    setupUi();
}

AboutInfo::~AboutInfo()
{
}

void AboutInfo::setupUi()
{
    setWindowTitle("About MediaSonic");
    setFixedSize(400, 250);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel(this);
    QPixmap iconPixmap(":/gfx/MediaSonic.png");
    iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    QLabel *titleLabel = new QLabel("MediaSonic", this);
    QFont tf = titleLabel->font();
    tf.setPointSize(tf.pointSize() + 4);
    tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QLabel *versionLabel = new QLabel("Version 1.0 alpha", this);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

    layout->addStretch();

    QLabel *copyrightLabel = new QLabel("™ and © 2025 Syndromatic Ltd. All rights reserved.", this);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(copyrightLabel);

    QPushButton *okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    setLayout(layout);
}
