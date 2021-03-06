#include "chip.h"
#include <QtGui>

Chip::Chip(const QColor &color, int x, int y)
{
    this->x = x;
    this->y = y;
    this->color = color;
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable | ItemIsMovable);
    // ইমেইজ সিলেক্ট এবং মুভ করা যাবে।
    setAcceptsHoverEvents(true);
    // ইমেইজের উপর মাউস রাখলে ইফেক্ট দেখাবে।
}

QRectF Chip::boundingRect() const
{
    return QRectF(0, 0, 110, 70);
    // চিপগুলো উইন্ডোর কোন অবস্থানে থাকবে তা নির্দেশ করে।
}

QPainterPath Chip::shape() const
{
    QPainterPath path;
    // এটা দিয়ে বিভিন্ন আকার যেমন সার্কেল, আয়তাকার বিভিন্ন কিছু দেয়া যায়।
    path.addRect(14, 14, 82, 42);
    // এটা এখানে কি কাজ করছে বোঝা যাচ্ছে না।
    return path;
}

void Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? color.dark(150) : color;
    // মাউস যদি চিপের উপর সিলেক্ট করে তাহলে এটা একটু অন্ধাকার হয়ে যাবে।
    if (option->state & QStyle::State_MouseOver)
        fillColor = fillColor.light(125);
    // মাউস রাখলেই উজ্জলতা দেখাবে
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    // levelOfDetailFromTransform ফাংশন সম্ভবত জুম আউট করার সময় কাজ করে।
    if (lod < 0.2) {
        if (lod < 0.125) {
            painter->fillRect(QRectF(0, 0, 110, 70), fillColor);
            // এটা পরিবর্তনে জুম আউটের মান পরিবর্তিত হয়। 
            return;
        }

        QBrush b = painter->brush();
        painter->setBrush(fillColor);
        painter->drawRect(13, 13, 97, 57);
        // জুমের মান যত কমানো হবে চিপের ছবিগুলো যাতে খুব সুন্দর করে জুম আউট হয়, 
        // তাই এরমান প্রথমে .2 পরে .125 ব্যবহার করা হয়েছে।
        painter->setBrush(b);
        return;
    }

    QPen oldPen = painter->pen();
    QPen pen = oldPen;
    int width = 0;
    if (option->state & QStyle::State_Selected)
        width += 2;
        // এর পরিবর্তনে এপ্লিকেশনে খুব একটা বেশি পরিবর্তন দেখতে পাচ্ছি না।
    pen.setWidth(width);
    QBrush b = painter->brush();
    painter->setBrush(QBrush(fillColor.dark(option->state & QStyle::State_Sunken ? 120 : 100)));
    painter->drawRect(QRect(14, 14, 79, 39));
    // এটা চিপগুলোর সবুজ রঙ নির্দেশ করে।
    painter->setBrush(b);

    if (lod >= 1) {
        painter->setPen(QPen(Qt::gray, 1));
        painter->drawLine(15, 54, 94, 54);
        painter->drawLine(94, 53, 94, 15);
        painter->setPen(QPen(Qt::black, 0));
    }

    // Draw text
    if (lod >= 2) {
        QFont font("Times", 10);
        font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);
        painter->save();
        painter->scale(0.1, 0.1);
        painter->drawText(170, 180, QString("Model: VSC-2000 (Very Small Chip) at %1x%2").arg(x).arg(y));
        painter->drawText(170, 200, QString("Serial number: DLWR-WEER-123L-ZZ33-SDSJ"));
        painter->drawText(170, 220, QString("Manufacturer: Chip Manufacturer"));
        // চিপের মধ্যে লেখাটি লিখবে।
        painter->restore();
    }

    // Draw lines
    QVarLengthArray<QLineF, 36> lines;
    if (lod >= 0.5) {
        for (int i = 0; i <= 10; i += (lod > 0.5 ? 1 : 2)) {
            lines.append(QLineF(18 + 7 * i, 13, 18 + 7 * i, 5));
            lines.append(QLineF(18 + 7 * i, 54, 18 + 7 * i, 62));
            // লম্বালম্বিভাবে চিপের পাশে সার্কিটের মত দাগ দাগ আকার দেয়া হয়।
        }
        for (int i = 0; i <= 6; i += (lod > 0.5 ? 1 : 2)) {
            lines.append(QLineF(5, 18 + i * 5, 13, 18 + i * 5));
            lines.append(QLineF(94, 18 + i * 5, 102, 18 + i * 5));
            // পাশাপাশিভাবে চিপের পাশে সার্কিটের মত দাগ দাগ আকার দেয়া হয়।
        }
    }
    if (lod >= 0.4) {
        const QLineF lineData[] = {
            QLineF(25, 35, 35, 35),
            QLineF(35, 30, 35, 40),
            QLineF(35, 30, 45, 35),
            QLineF(35, 40, 45, 35),
            QLineF(45, 30, 45, 40),
            QLineF(45, 35, 55, 35)
            // চিপের ভেতর সার্কিটের গেইট আকার দেয়ার জন্য ।
        };
        lines.append(lineData, 6);
    }
    painter->drawLines(lines.data(), lines.size());

    // Draw red ink
    if (stuff.size() > 1) {
        QPen p = painter->pen();
        painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo(stuff.first());
        for (int i = 1; i < stuff.size(); ++i)
            path.lineTo(stuff.at(i));
        painter->drawPath(path);
        painter->setPen(p);
        // যখন Shift চাপ দিয়ে মাউস দিয়ে কোন লাইন ড্র করি তখন লাল লাইন আসবে।
    }
}

void Chip::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}

void Chip::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        stuff << event->pos();
        update();
        return;
     // শিফট এর কাজ করানোর জন্য Shift modifier ইউজ করা হয়।
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void Chip::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}
