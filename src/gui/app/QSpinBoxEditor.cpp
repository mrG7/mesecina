//#include <QtGui>
#include <iostream>

#include <gui/app/QSpinBoxEditor.h>
#include <gui/app/Settings_table_model.h>

QSpinBoxEditor::QSpinBoxEditor(Settings_table_model *table_model, QObject *parent)
: QItemDelegate(parent)
{
	this->table_model = table_model;
	current_index = new QModelIndex();
}

QWidget *QSpinBoxEditor::createEditor(QWidget *parent,
									  const QStyleOptionViewItem & option ,
									  const QModelIndex & index ) const
{
	if (index.model()->data(index, Qt::DisplayRole).type() == QVariant::Int) {
		QSpinBox *editor = new QSpinBox(parent);
		editor->setMinimum(0);
		editor->setMaximum(900000);
		editor->installEventFilter(const_cast<QSpinBoxEditor*>(this));
		connect(editor, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
		*current_index = index;
		return editor;
	} else return QItemDelegate::createEditor(parent,
		option,
		index );
		
}

void QSpinBoxEditor::setEditorData(QWidget *editor,
								   const QModelIndex &index) const
{
//	std::cout << index.model()->data(index, Qt::DisplayRole).type() << std::endl;
	if (index.model()->data(index, Qt::DisplayRole).type() == QVariant::Int) {
		int value = index.model()->data(index, Qt::DisplayRole).toInt();

		QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(value);
	} else QItemDelegate::setEditorData(editor, index);
}

void QSpinBoxEditor::setModelData(QWidget *editor, QAbstractItemModel *model,
								  const QModelIndex &index) const
{
	if (index.model()->data(index, Qt::DisplayRole).type() == QVariant::Int) {
		QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
		spinBox->interpretText();
		int value = spinBox->value();

		model->setData(index, value);
	} else QItemDelegate::setModelData(editor, model, index);
}

void QSpinBoxEditor::updateEditorGeometry(QWidget *editor,
										  const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void QSpinBoxEditor::valueChanged(int i) {
//	std::cout << "value changed "<< i<< std::endl;
	table_model->setData(*current_index,i);
}