#include "texteditor.h"
#include "mainwindow.h"

#include <QDebug>
#include <QTextStream>
#include <QtWidgets>

TextEditor::TextEditor(QWidget *parent)
	: QTextEdit(parent)
{
	// https://www.binpress.com/tutorial/building-a-text-editor-with-pyqt-part-3/147
	setFont(QFont("Sans Serif", 24));
	connect(this, &TextEditor::textChanged, this, &TextEditor::on_pageTextChanged);

	lineNumberArea = new LineNumberArea(this);

	connect(this->document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateLineNumberArea /*_2*/ (int)));
	connect(this, SIGNAL(textChanged()), this, SLOT(updateLineNumberArea()));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateLineNumberArea()));
	//connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	//highlightCurrentLine();
}

void TextEditor::on_pageTextChanged()
{
	emit plainTextChanged(toPlainText());
}

void TextEditor::on_actionWrapLine_toggled(bool wrapLine)
{
	if (wrapLine) {
		setLineWrapMode(QTextEdit::WidgetWidth);
	} else {
		setLineWrapMode(QTextEdit::NoWrap);
	}
}

void TextEditor::searchedTextChanged(const QString searchedText)
{
	QTextDocument *document = this->document();
	QList<QTextCursor>::iterator it;

	QTextCursor cursor(document);
	QTextCursor highlightCursor(document);
	QTextCharFormat plainFormat(highlightCursor.charFormat());
	QTextCharFormat colorFormat = plainFormat;
	bool found = false;

	// ak je text uz zvyrazeny a zacnem mazat hladane slovo
	if (!mSelection.empty()) {
		colorFormat.setBackground(Qt::white);
		for (it = mSelection.begin(); it != mSelection.end(); it++) {
			(*it).mergeCharFormat(colorFormat);
		}
		mSelection.clear();
	}

	colorFormat.setBackground(Qt::yellow);
	if (!searchedText.isEmpty()) {
		cursor.beginEditBlock();
		while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
			highlightCursor = document->find(searchedText, highlightCursor);

			if (!highlightCursor.isNull()) {
				found = true;
				highlightCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 0);
				highlightCursor.mergeCharFormat(colorFormat);
				mSelection.append(highlightCursor);
			}
		}
		cursor.endEditBlock();

		if (found) {
			emit textFound(true);
		} else {
			emit textFound(false);
			found = false;
		}
	} else {
		emit textFound(true);
		found = false;
	}

	if (found == false) {
		colorFormat.setBackground(Qt::white);
		for (it = mSelection.begin(); it != mSelection.end(); it++) {
			(*it).mergeCharFormat(colorFormat);
		}
		mSelection.clear();
	}
}

void TextEditor::findNextText(const QString &text)
{
	if (!text.isEmpty()) {
		if (find(text) == false) {
			QTextCursor textCursor = this->textCursor();
			textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 0);
			setTextCursor(textCursor);
		}
	}
}

void TextEditor::findPreviousText(const QString &text)
{
	if (!text.isEmpty()) {
		if (find(text, QTextDocument::FindBackward) == false) {
			QTextCursor textCursor = this->textCursor();
			textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 0);
			setTextCursor(textCursor);
		}
	}
}

void TextEditor::replaceNextText(const QString &search, const QString &replace)
{
	QTextCursor textCursor(this->textCursor());

	if (textCursor.atEnd()) {
		textCursor.setPosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		this->setTextCursor(textCursor);
	}

	if (!search.isEmpty()) {
		if (!textCursor.selectedText().isEmpty()) {

			if (textCursor.selectedText().compare(search, Qt::CaseInsensitive) == 0) {
				QTextCharFormat colorFormat;
				colorFormat.setBackground(Qt::white);
				textCursor.mergeCharFormat(colorFormat);

				insertPlainText(replace);
			}
		}

		if (find(search) == true) {
		}
	}
}

void TextEditor::replaceAllText(const QString &search, const QString &replace)
{
	QTextCursor cursor(textCursor());

	if (!search.isEmpty()) {
		QString text = toPlainText();
		text.replace(search, replace);

		cursor.select(QTextCursor::Document);
		setTextCursor(cursor);

		insertPlainText(text);
	}
}

// ----------------------------------------------------------------------- //
//http://stackoverflow.com/questions/2443358/how-to-add-lines-numbers-to-qtextedit
// Funkcia vycleni priesto pre panel na riadky v texteditore o danej velkosti
void TextEditor::updateLineNumberAreaWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

int TextEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, this->document()->blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 10 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

// Funkcia rozsiruje panel pre riadky
void TextEditor::resizeEvent(QResizeEvent *e)
{
	QTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//void TextEditor::highlightCurrentLine()
//{
//    QList<QTextEdit::ExtraSelection> extraSelections;

//    if (!isReadOnly()) {
//        QTextEdit::ExtraSelection selection;

//        QColor lineColor = QColor(Qt::yellow).lighter(160);

//        selection.format.setBackground(lineColor);
//        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
//        selection.cursor = textCursor();
//        selection.cursor.clearSelection();
//        extraSelections.append(selection);
//    }

//    setExtraSelections(extraSelections);
//}

void TextEditor::updateLineNumberArea(QRectF /*rect_f*/)
{
	TextEditor::updateLineNumberArea();
}
void TextEditor::updateLineNumberArea(int /*slider_pos*/)
{
	TextEditor::updateLineNumberArea();
}

void TextEditor::updateLineNumberArea()
{
	/*
     * When the signal is emitted, the sliderPosition has been adjusted according to the action,
     * but the value has not yet been propagated (meaning the valueChanged() signal was not yet emitted),
     * and the visual display has not been updated. In slots connected to this signal you can thus safely
     * adjust any action by calling setSliderPosition() yourself, based on both the action and the
     * slider's value.
     */
	// Make sure the sliderPosition triggers one last time the valueChanged() signal with the actual value !!!!
	this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->sliderPosition());

	// Since "QTextEdit" does not have an "updateRequest(...)" signal, we chose
	// to grab the imformations from "sliderPosition()" and "contentsRect()".
	// See the necessary connections used (Class constructor implementation part).

	QRect rect = this->contentsRect();
	lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	updateLineNumberAreaWidth(0);
	//----------
	int dy = this->verticalScrollBar()->sliderPosition();
	if (dy > -1) {
		lineNumberArea->scroll(0, dy);
	}

	// Addjust slider to alway see the number of the currently being edited line...
	int first_block_id = getFirstVisibleBlockId();
	if (first_block_id == 0 || this->textCursor().block().blockNumber() == first_block_id - 1)
		this->verticalScrollBar()->setSliderPosition(dy - this->document()->documentMargin());

	// Snap to first line (TODO...)
	//    if (first_block_id > 0)
	//    {
	//        int slider_pos = this->verticalScrollBar()->sliderPosition();
	//        int prev_block_height = (int) this->document()->documentLayout()->blockBoundingRect(this->document()->findBlockByNumber(first_block_id-1)).height();
	//        if( dy <= this->document()->documentMargin() + prev_block_height )
	//        {
	//            this->verticalScrollBar()->setSliderPosition(slider_pos - (this->document()->documentMargin() + prev_block_height));
	//        }
	//    }
}

int TextEditor::getFirstVisibleBlockId()
{
	// Detect the first block for which bounding rect - once translated
	// in absolute coordinated - is contained by the editor's text area

	// Costly way of doing but since "blockBoundingGeometry(...)" doesn't
	// exists for "QTextEdit"...

	QTextCursor curs = QTextCursor(this->document());
	curs.movePosition(QTextCursor::Start);
	for (int i = 0; i < this->document()->blockCount(); ++i) {
		QTextBlock block = curs.block();

		QRect r1 = this->viewport()->geometry();
		QRect r2 = this->document()->documentLayout()->blockBoundingRect(block).translated(
																				   this->viewport()->geometry().x(), this->viewport()->geometry().y() - (this->verticalScrollBar()->sliderPosition()))
					   .toRect();

		if (r1.contains(r2, true)) {
			return i;
		}

		curs.movePosition(QTextCursor::NextBlock);
	}

	return 0;
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->sliderPosition());

	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), QColor(240, 240, 240));
	int blockNumber = this->getFirstVisibleBlockId();

	QTextBlock block = this->document()->findBlockByNumber(blockNumber);
	QTextBlock prev_block = (blockNumber > 0) ? this->document()->findBlockByNumber(blockNumber - 1) : block;
	int translate_y = (blockNumber > 0) ? -this->verticalScrollBar()->sliderPosition() : 0;

	int top = this->viewport()->geometry().top();

	// Adjust text position according to the previous "non entirely visible" block
	// if applicable. Also takes in consideration the document's margin offset.
	int additional_margin;
	if (blockNumber == 0) {
		// Simply adjust to document's margin
		additional_margin = (int)this->document()->documentMargin() - 1 - this->verticalScrollBar()->sliderPosition();
	} else {
		// Getting the height of the visible part of the previous "non entirely visible" block
		additional_margin = (int)this->document()->documentLayout()->blockBoundingRect(prev_block).translated(0, translate_y).intersected(this->viewport()->geometry()).height();
	}

	// Shift the starting point
	top += additional_margin;

	int bottom = top + (int)this->document()->documentLayout()->blockBoundingRect(block).height();

	QColor col_1(0, 50, 200); // Current line
	QColor col_0(0, 0, 0); // Other lines

	// Draw the numbers (displaying the current line number in green)
	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(QColor(120, 120, 120));
			painter.setPen((this->textCursor().blockNumber() == blockNumber) ? col_1 : col_0);
			painter.drawText(-5, top,
				lineNumberArea->width(), fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)this->document()->documentLayout()->blockBoundingRect(block).height();
		++blockNumber;
	}
}
