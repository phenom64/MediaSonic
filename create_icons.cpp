#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create icons directory
    QDir().mkpath("src/gfx/icons");
    
    struct IconInfo {
        QString text;
        QString filename;
    };
    
    QVector<IconInfo> icons = {
        {"M", "music"},
        {"MOV", "movies"},
        {"TV", "tv"},
        {"POD", "podcasts"},
        {"AUD", "audiobooks"},
        {"REC", "recent"},
        {"RP", "recent_played"},
        {"TOP", "top25"},
        {"▶", "play"},
        {"⏸", "pause"},
        {"⏹", "stop"},
        {"⏮", "previous"},
        {"⏭", "next"},
        {"🔊", "volume"},
        {"🔍", "search"},
        {"📋", "list_view"},
        {"📁", "album_view"},
        {"🎵", "cover_flow"},
        {"SYN", "syndromatic_logo"},
        {"📻", "radio"},
        {"🏠", "home_sharing"},
        {"🧠", "genius"},
        {"🎧", "dj"},
        {"90s", "90s"},
        {"🎼", "classical"},
        {"🎬", "videos"},
        {"⭐", "top_rated"},
        {"📁", "folder"},
        {"📝", "playlist"}
    };
    
    for (const auto &icon : icons) {
        QPixmap pixmap(32, 32);
        pixmap.fill(QColor(240, 240, 240));
        
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw border
        painter.setPen(QPen(QColor(180, 180, 180), 1));
        painter.drawRect(0, 0, 31, 31);
        
        // Draw text
        painter.setPen(QColor(80, 80, 80));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        
        QRect textRect = pixmap.rect();
        painter.drawText(textRect, Qt::AlignCenter, icon.text);
        
        QString filename = QString("src/gfx/icons/%1.png").arg(icon.filename);
        if (pixmap.save(filename)) {
            qDebug() << "Created" << filename;
        } else {
            qDebug() << "Failed to create" << filename;
        }
    }
    
    qDebug() << "All icons created successfully!";
    return 0;
} 