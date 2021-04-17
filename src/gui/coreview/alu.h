#ifndef COREVIEW_ALU_H
#define COREVIEW_ALU_H

#include "connection.h"

#include <QGraphicsItem>

namespace coreview {

class Alu : public QGraphicsItem {
public:
    Alu();
    ~Alu() override;

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

    void setPos(qreal x, qreal y);
    const Connector *connector_in_a() const;
    const Connector *connector_in_b() const;
    const Connector *connector_out() const;
    const Connector *connector_ctl() const;

private:
    QGraphicsSimpleTextItem name;
    Connector *con_in_a, *con_in_b, *con_out, *con_ctl;
};

} // namespace coreview

#endif // COREVIEW_ALU_H
