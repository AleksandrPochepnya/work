#ifndef WAITINGSPINNERWIDGET_HPP
#define WAITINGSPINNERWIDGET_HPP


#include <QWidget>
#include <QColor>

class QTimer;

class WaitingSpinnerWidget : public QWidget
{
    Q_OBJECT
public:
    /*! Constructor for "standard" widget behaviour - use this
   * constructor if you wish to, e.g. embed your widget in another. */
    WaitingSpinnerWidget(QWidget *parent = nullptr,
                         bool centerOnParent = true,
                         bool disableParentWhenSpinning = true);

    /*! Constructor - use this constructor to automatically create a modal
   * ("blocking") spinner on top of the calling widget/window.  If a valid
   * parent widget is provided, "centreOnParent" will ensure that
   * QtWaitingSpinner automatically centres itself on it, if not,
   * "centreOnParent" is ignored. */
    WaitingSpinnerWidget(Qt::WindowModality modality,
                         QWidget *parent = nullptr,
                         bool centerOnParent = true,
                         bool disableParentWhenSpinning = true);

public slots:
    void start();
    void stop();

public:
    void setColor(QColor color);
    void setRoundness(qreal roundness);
    void setMinimumTrailOpacity(qreal minimumTrailOpacity);
    void setTrailFadePercentage(qreal trail);
    void setRevolutionsPerSecond(qreal revolutionsPerSecond);
    void setNumberOfLines(int lines);
    void setLineLength(int length);
    void setLineWidth(int width);
    void setInnerRadius(int radius);
    void setText(QString text);

    QColor color();
    qreal roundness();
    qreal minimumTrailOpacity();
    qreal trailFadePercentage();
    qreal revolutionsPersSecond();
    int numberOfLines();
    int lineLength();
    int lineWidth();
    int innerRadius();

    bool isSpinning() const;
    void updatePosition();

private slots:
    void rotate();

protected:
    void paintEvent(QPaintEvent *paintEvent);

private:
    static int lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines);
    static QColor currentLineColor(int distance, int totalNrOfLines,
                                   qreal trailFadePerc, qreal minOpacity,
                                   QColor color);
    void initialize();
    void updateSize();
    void updateTimer();

private:
    QColor mColor;
    qreal mRoundness = 0; // 0..100
    qreal mMinimumTrailOpacity = 0;
    qreal mTrailFadePercentage = 0;
    qreal mRevolutionsPerSecond = 0;
    int mNumberOfLines = 0;
    int mLineLength = 0;
    int mLineWidth = 0;
    int mInnerRadius = 0;
    QString mText;

private:
    WaitingSpinnerWidget(const WaitingSpinnerWidget&);
    WaitingSpinnerWidget& operator=(const WaitingSpinnerWidget&);

    QTimer *mTimer = nullptr;
    bool mCenterOnParent = true;
    bool mDisableParentWhenSpinning = true;
    int  mCurrentCounter = 0;
    bool mIsSpinning = false;
};


#endif // WAITINGSPINNERWIDGET_HPP
