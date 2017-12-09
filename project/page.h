#ifndef PAGE_H
#define PAGE_H

#include "qstring.h"
#include "qpixmap.h"
#include "qpixmapcache.h"

class Path;

class Page
{
	friend class Pages;

public:
	explicit Page(const Path *path);

	QString plainText() const;
	void setPlainText(const QString &text);
	int index() const;
	QPixmap image() const;
	void deleteImage();

private:
	void save();

	const Path *m_path;
	QString m_text;
};

#endif // PAGE_H
