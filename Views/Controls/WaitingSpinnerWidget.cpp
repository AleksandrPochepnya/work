#include "WaitingSpinnerWidget.hpp"

#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QTimer>

WaitingSpinnerWidget::WaitingSpinnerWidget(QWidget *parent,
                                           bool centerOnParent,
                                           bool disableParentWhenSpinning)
    : QWidget(parent),
      mCenterOnParent(centerOnParent),
      mDisableParentWhenSpinning(disableParentWhenSpinning)
{
    initialize();
}

WaitingSpinnerWidget::WaitingSpinnerWidget(Qt::WindowModality modality,
                                           QWidget *parent,
                                           bool centerOnParent,
                                           bool disableParentWhenSpinning)
    : QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint),
      mCenterOnParent(centerOnParent),
      mDisableParentWhenSpinning(disableParentWhenSpinning)
{
    initialize();

    // We need to set the window modality AFTER we've hidden the
    // widget for the first time since changing this property while
    // the widget is visible has no effect.
    setWindowModality(modality);
    setAttribute(Qt::WA_TranslucentBackground);
}

void WaitingSpinnerWidget::initialize()
{
    mColor = Qt::gray;
    mRoundness = 100.0;
    mMinimumTrailOpacity = 3.14159265358979323846;
    mTrailFadePercentage = 80.0;
    mRevolutionsPerSecond = 1.57079632679489661923;
    mNumberOfLines = 20;
    mLineLength = 5;
    mLineWidth = 2;
    mInnerRadius = 5;
    mCurrentCounter = 0;
    mIsSpinning = false;

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &WaitingSpinnerWidget::rotate);
    updateSize();
    updateTimer();
    hide();
}

void WaitingSpinnerWidget::paintEvent(QPaintEvent *)
{
    updatePosition();
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::transparent);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (mCurrentCounter >= mNumberOfLines)
    {
        mCurrentCounter = 0;
    }

    painter.setPen(Qt::NoPen);
    for (int i = 0; i < mNumberOfLines; ++i)
    {
        painter.save();
        painter.translate(mInnerRadius + mLineLength,
                          mInnerRadius + mLineLength);
        qreal rotateAngle = static_cast<qreal>(360 * i) / static_cast<qreal>(mNumberOfLines);
        painter.rotate(rotateAngle);
        painter.translate(mInnerRadius, 0);
        int distance = lineCountDistanceFromPrimary(i, mCurrentCounter, mNumberOfLines);
        QColor color = currentLineColor(distance, mNumberOfLines,
                                        mTrailFadePercentage, mMinimumTrailOpacity, mColor);
        painter.setBrush(color);
        // TODO improve the way rounded rect is painted
        painter.drawRoundedRect(QRect(0, -mLineWidth / 2, mLineLength, mLineWidth),
                                mRoundness, mRoundness, Qt::RelativeSize);
        painter.restore();
    }
}

void WaitingSpinnerWidget::start()
{
    updatePosition();
    mIsSpinning = true;
    show();

    if(parentWidget() && mDisableParentWhenSpinning)
    {
        parentWidget()->setEnabled(false);
    }

    if (!mTimer->isActive())
    {
        mTimer->start();
        mCurrentCounter = 0;
    }
}

void WaitingSpinnerWidget::stop()
{
    mIsSpinning = false;
    hide();

    if(parentWidget() && mDisableParentWhenSpinning)
    {
        parentWidget()->setEnabled(true);
    }

    if (mTimer->isActive())
    {
        mTimer->stop();
        mCurrentCounter = 0;
    }
}

void WaitingSpinnerWidget::setNumberOfLines(int lines)
{
    mNumberOfLines = lines;
    mCurrentCounter = 0;
    updateTimer();
}

void WaitingSpinnerWidget::setLineLength(int length)
{
    mLineLength = length;
    updateSize();
}

void WaitingSpinnerWidget::setLineWidth(int width)
{
    mLineWidth = width;
    updateSize();
}

void WaitingSpinnerWidget::setInnerRadius(int radius)
{
    mInnerRadius = radius;
    updateSize();
}

void WaitingSpinnerWidget::setText(QString text)
{
    mText = text;
}

QColor WaitingSpinnerWidget::color()
{
    return mColor;
}

qreal WaitingSpinnerWidget::roundness()
{
    return mRoundness;
}

qreal WaitingSpinnerWidget::minimumTrailOpacity()
{
    return mMinimumTrailOpacity;
}

qreal WaitingSpinnerWidget::trailFadePercentage()
{
    return mTrailFadePercentage;
}

qreal WaitingSpinnerWidget::revolutionsPersSecond()
{
    return mRevolutionsPerSecond;
}

int WaitingSpinnerWidget::numberOfLines()
{
    return mNumberOfLines;
}

int WaitingSpinnerWidget::lineLength()
{
    return mLineLength;
}

int WaitingSpinnerWidget::lineWidth()
{
    return mLineWidth;
}

int WaitingSpinnerWidget::innerRadius()
{
    return mInnerRadius;
}

bool WaitingSpinnerWidget::isSpinning() const
{
    return mIsSpinning;
}

void WaitingSpinnerWidget::setRoundness(qreal roundness)
{
    mRoundness = std::max(0.0, std::min(100.0, roundness));
}

void WaitingSpinnerWidget::setColor(QColor color)
{
    mColor = color;
}

void WaitingSpinnerWidget::setRevolutionsPerSecond(qreal revolutionsPerSecond)
{
    mRevolutionsPerSecond = revolutionsPerSecond;
    updateTimer();
}

void WaitingSpinnerWidget::setTrailFadePercentage(qreal trail)
{
    mTrailFadePercentage = trail;
}

void WaitingSpinnerWidget::setMinimumTrailOpacity(qreal minimumTrailOpacity)
{
    mMinimumTrailOpacity = minimumTrailOpacity;
}

void WaitingSpinnerWidget::rotate()
{
    ++mCurrentCounter;
    if (mCurrentCounter >= mNumberOfLines)
    {
        mCurrentCounter = 0;
    }
    update();
}

void WaitingSpinnerWidget::updateSize()
{
    int size = (mInnerRadius + mLineLength) * 2;
    setFixedSize(size, size);
}

void WaitingSpinnerWidget::updateTimer()
{
    mTimer->setInterval(1000 / (mNumberOfLines * mRevolutionsPerSecond));
}

void WaitingSpinnerWidget::updatePosition()
{
    if (parentWidget() && mCenterOnParent)
    {
        move(parentWidget()->width() / 2 - width() / 2,
             parentWidget()->height() / 2 - height() / 2);
    }
}

int WaitingSpinnerWidget::lineCountDistanceFromPrimary(int current, int primary,
                                                       int totalNrOfLines)
{
    int distance = primary - current;
    if (distance < 0)
    {
        distance += totalNrOfLines;
    }

    return distance;
}

QColor WaitingSpinnerWidget::currentLineColor(int countDistance, int totalNrOfLines,
                                              qreal trailFadePerc, qreal minOpacity,
                                              QColor color)
{
    if (countDistance == 0)
    {
        return color;
    }
    const qreal minAlphaF = minOpacity / 100.0;
    int distanceThreshold = static_cast<int>(ceil((totalNrOfLines - 1) * trailFadePerc / 100.0));
    if (countDistance > distanceThreshold)
    {
        color.setAlphaF(minAlphaF);
    }
    else
    {
        qreal alphaDiff = color.alphaF() - minAlphaF;
        qreal gradient = alphaDiff / static_cast<qreal>(distanceThreshold + 1);
        qreal resultAlpha = color.alphaF() - gradient * countDistance;

        // If alpha is out of bounds, clip it.
        resultAlpha = std::min(1.0, std::max(0.0, resultAlpha));
        color.setAlphaF(resultAlpha);
    }

    return color;
}
