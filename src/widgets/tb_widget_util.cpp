#include "tb_widget_util.h"

#include <QAbstractSpinBox>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSizePolicy>
#include <QSpinBox>

namespace TbWidget {

namespace {

void configureFormLabel(QLabel *label, int rowHeight)
{
    if (!label) {
        return;
    }
    const int h = rowHeight > 0 ? rowHeight : kFormFieldHeight;
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setFixedHeight(h);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

bool isSingleRowContainer(const QWidget *widget)
{
    return widget && widget->layout() && qobject_cast<const QHBoxLayout *>(widget->layout());
}

int rowContainerHeight(QWidget *row)
{
    int rowH = kFormFieldHeight;
    if (!row) {
        return rowH;
    }
    if (QLayout *layout = row->layout()) {
        for (int i = 0; i < layout->count(); ++i) {
            if (QWidget *child = layout->itemAt(i)->widget()) {
                rowH = qMax(rowH, styledControlHeight(child));
            }
        }
    }
    return rowH;
}

} // namespace

int styledControlHeight(QWidget *widget)
{
    if (!widget) {
        return kFormFieldHeight;
    }
    widget->ensurePolished();
    int h = qMax(kFormFieldHeight, widget->sizeHint().height());
  if (const auto *spin = qobject_cast<const QAbstractSpinBox *>(widget)) {
        h = qMax(h, spin->minimumSizeHint().height());
    }
    return h;
}

QLabel *createFormLabel(QWidget *parent, const QString &text, int rowHeight)
{
    QLabel *label = new QLabel(text, parent);
    configureFormLabel(label, rowHeight > 0 ? rowHeight : kFormFieldHeight);
    return label;
}

void initFormRowWidget(QWidget *row)
{
    if (!row) {
        return;
    }
    const int rowH = rowContainerHeight(row);
    row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row->setFixedHeight(rowH);
    if (QLayout *layout = row->layout()) {
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setAlignment(Qt::AlignVCenter);
    }
}

FormGridBuilder::FormGridBuilder(QWidget *parent)
    : m_parent(parent)
    , m_grid(new QGridLayout(parent))
{
    m_grid->setContentsMargins(12, 16, 12, 12);
    m_grid->setHorizontalSpacing(kFormHorizontalSpacing);
    m_grid->setVerticalSpacing(kFormRowVerticalSpacing);
    m_grid->setColumnStretch(1, 1);
    m_grid->setAlignment(Qt::AlignTop);
}

FormGridBuilder::~FormGridBuilder()
{
    if (!m_grid || m_row <= 0) {
        return;
    }
    for (int r = 0; r < m_row; ++r) {
        m_grid->setRowStretch(r, 0);
    }
    m_grid->setRowStretch(m_row, 1);
}

int FormGridBuilder::fieldRowHeight(QWidget *field) const
{
    if (!field) {
        return kFormFieldHeight;
    }
    if (isSingleRowContainer(field)) {
        return rowContainerHeight(field);
    }
    return styledControlHeight(field);
}

void FormGridBuilder::prepareField(QWidget *field)
{
    if (!field) {
        return;
    }
    if (isSingleRowContainer(field)) {
        initFormRowWidget(field);
        return;
    }
    const int h = styledControlHeight(field);
    field->setFixedHeight(h);
    field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void FormGridBuilder::addLabeledRow(const QString &labelText, QWidget *field)
{
    if (!m_grid || !m_parent || !field) {
        return;
    }
    prepareField(field);
    const int rowH = fieldRowHeight(field);
    m_grid->addWidget(createFormLabel(m_parent, labelText, rowH), m_row, 0, Qt::AlignTop);
    m_grid->addWidget(field, m_row, 1, Qt::AlignTop);
    ++m_row;
}

void FormGridBuilder::addFieldRow(QWidget *field)
{
    if (!m_grid || !field) {
        return;
    }
    prepareField(field);
    m_grid->addWidget(field, m_row, 1, Qt::AlignTop);
    ++m_row;
}

void applyComboBoxStyle(QComboBox *box)
{
    if (!box) {
        return;
    }
    const int h = styledControlHeight(box);
    box->setFixedHeight(h);
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applyComboBoxStyles(QWidget *root, int maxPopupHeightPx)
{
    if (!root || maxPopupHeightPx <= 0) {
        return;
    }

    const QList<QComboBox *> boxes = root->findChildren<QComboBox *>();
    int rowHeight = kFormFieldHeight;
    if (!boxes.isEmpty()) {
        rowHeight = styledControlHeight(boxes.first());
    }
    const int maxItems = qMax(6, maxPopupHeightPx / rowHeight);

    for (QComboBox *box : boxes) {
        box->setMaxVisibleItems(maxItems);
        applyComboBoxStyle(box);
    }
}

void applyLineEditStyle(QLineEdit *edit)
{
    if (!edit) {
        return;
    }
    const int h = styledControlHeight(edit);
    edit->setFixedHeight(h);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applySpinBoxStyle(QSpinBox *spin)
{
    if (!spin) {
        return;
    }
    const int h = styledControlHeight(spin);
    spin->setFixedHeight(h);
    spin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void applyFormFieldStyles(QWidget *root)
{
    if (!root) {
        return;
    }
    const QList<QLineEdit *> edits = root->findChildren<QLineEdit *>();
    for (QLineEdit *edit : edits) {
        applyLineEditStyle(edit);
    }
    const QList<QSpinBox *> spins = root->findChildren<QSpinBox *>();
    for (QSpinBox *spin : spins) {
        applySpinBoxStyle(spin);
    }
}

} // namespace TbWidget
