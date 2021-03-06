/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  generator.h
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "node.h"
#include "text.h"

QT_BEGIN_NAMESPACE

typedef QMap<QString, NodeMap> NewClassMaps;
typedef QMap<QString, NodeMultiMap> NewSinceMaps;
typedef QMap<QString, const Node*> NodeMap;
typedef QMultiMap<QString, Node*> NodeMultiMap;
typedef QMap<Node*, NodeMultiMap> ParentMaps;

class ClassNode;
class Config;
class CodeMarker;
class DocNode;
class FunctionNode;
class InnerNode;
class Location;
class NamespaceNode;
class Node;
class Tree;

class Generator
{
public:
    Generator();
    virtual ~Generator();

    virtual bool canHandleFormat(const QString &format) { return format == this->format(); }
    virtual QString format() = 0;
    virtual void generateTree(Tree *tree);
    virtual void initializeGenerator(const Config &config);
    virtual void terminateGenerator();

    QString fullDocumentLocation(const Node *node, bool subdir = false);

    static const QString& baseDir() { return baseDir_; }
    static Generator *currentGenerator() { return currentGenerator_; }
    static Generator *generatorForFormat(const QString& format);
    static void initialize(const Config& config);
    static const QString& outputDir() { return outDir_; }
    static void terminate();
    static void writeOutFileNames();

protected:
    virtual void beginSubPage(const InnerNode* node, const QString& fileName);
    virtual void endSubPage();
    virtual void endText(const Node *relative, CodeMarker *marker);
    virtual QString fileBase(const Node* node) const;
    virtual QString fileExtension() const = 0;
    virtual QString fullName(const Node *node,
                             const Node *relative,
                             CodeMarker *marker) const;
    virtual void generateAlsoList(const Node *node, CodeMarker *marker);
    virtual int generateAtom(const Atom *atom,
                             const Node *relative,
                             CodeMarker *marker);
    virtual void generateBody(const Node *node, CodeMarker *marker);
    virtual void generateClassLikeNode(InnerNode* inner, CodeMarker* marker);
    virtual void generateDocNode(DocNode* dn, CodeMarker* marker);
    virtual void generateInheritedBy(const ClassNode *classe,
                                     CodeMarker *marker);
    virtual void generateInherits(const ClassNode *classe,
                                  CodeMarker *marker);
    virtual void generateInnerNode(InnerNode* node);
    virtual void generateMaintainerList(const InnerNode* node, CodeMarker* marker);
    virtual void generateQmlInheritedBy(const QmlClassNode* qcn, CodeMarker* marker);
    virtual void generateQmlInherits(const QmlClassNode* qcn, CodeMarker* marker);
    virtual bool generateQmlText(const Text& text,
                                 const Node *relative,
                                 CodeMarker *marker,
                                 const QString& qmlName);
    virtual bool generateText(const Text& text,
                              const Node *relative,
                              CodeMarker *marker);
    virtual QString imageFileName(const Node *relative, const QString& fileBase);
        virtual int skipAtoms(const Atom *atom, Atom::Type type) const;
    virtual void startText(const Node *relative, CodeMarker *marker);
    virtual QString typeString(const Node *node);

    static bool matchAhead(const Atom *atom, Atom::Type expectedAtomType);
    static QString outputPrefix(const QString &nodeType);
    static void singularPlural(Text& text, const NodeList& nodes);
    static void supplementAlsoList(const Node *node, QList<Text> &alsoList);
    static QString trimmedTrailing(const QString &string);
    static QString sinceTitles[];

    QString fileName(const Node* node) const;
    void findAllSince(const InnerNode *node);
    QMap<QString, QString> &formattingLeftMap();
    QMap<QString, QString> &formattingRightMap();
    const Atom* generateAtomList(const Atom *atom,
                                 const Node *relative,
                                 CodeMarker *marker,
                                 bool generate,
                                 int& numGeneratedAtoms);
    void generateExampleFiles(const DocNode *dn, CodeMarker *marker);
    void generateFileList(const DocNode* dn,
                          CodeMarker* marker,
                          Node::SubType subtype,
                          const QString& tag);
    void generateSince(const Node *node, CodeMarker *marker);
    void generateStatus(const Node *node, CodeMarker *marker);
    void generateThreadSafeness(const Node *node, CodeMarker *marker);
    QString getCollisionLink(const Atom* atom);
    QString getMetadataElement(const InnerNode* inner, const QString& t);
    QStringList getMetadataElements(const InnerNode* inner, const QString& t);
    QString indent(int level, const QString& markedCode);
    QTextStream& out();
    QString outFileName();
    bool parseArg(const QString& src,
                  const QString& tag,
                  int* pos,
                  int n,
                  QStringRef* contents,
                  QStringRef* par1 = 0,
                  bool debug = false);
    QString plainCode(const QString& markedCode);
    void setImageFileExtensions(const QStringList& extensions);
    void unknownAtom(const Atom *atom);
    void appendSortedQmlNames(Text& text,
                              const Node* base,
                              const NodeList& subs,
                              CodeMarker *marker);

    QList<NameCollisionNode*> collisionNodes;
    QMap<QString, QStringList> editionGroupMap;
    QMap<QString, QStringList> editionModuleMap;
    QString naturalLanguage;
    QTextCodec* outputCodec;
    QString outputEncoding;
    QStack<QTextStream*> outStreamStack;
    NewClassMaps newClassMaps;
    NewClassMaps newQmlClassMaps;
    NewSinceMaps newSinceMaps;

private:
    static QString baseDir_;
    static Generator* currentGenerator_;
    static QStringList exampleDirs;
    static QStringList exampleImgExts;
    static QMap<QString, QMap<QString, QString> > fmtLeftMaps;
    static QMap<QString, QMap<QString, QString> > fmtRightMaps;
    static QList<Generator *> generators;
    static QStringList imageDirs;
    static QStringList imageFiles;
    static QMap<QString, QStringList> imgFileExts;
    static QString project;
    static QString outDir_;
    static QSet<QString> outputFormats;
    static QHash<QString, QString> outputPrefixes;
    static QStringList scriptDirs;
    static QStringList scriptFiles;
    static QStringList styleDirs;
    static QStringList styleFiles;

    void appendFullName(Text& text,
                        const Node *apparentNode,
                        const Node *relative,
                        CodeMarker *marker,
                        const Node *actualNode = 0);
    void appendFullName(Text& text,
                        const Node *apparentNode,
                        const QString& fullName,
                        const Node *actualNode);
    void appendFullNames(Text& text,
                         const NodeList& nodes,
                         const Node* relative,
                         CodeMarker* marker);
    void appendSortedNames(Text& text,
                           const ClassNode *classe,
                           const QList<RelatedClass> &classes,
                           CodeMarker *marker);
    void generateReimplementedFrom(const FunctionNode *func,
                                   CodeMarker *marker);

    QString amp;
    QString gt;
    QString lt;
    QString quot;
    QRegExp tag;

 protected:
    Tree* tree_;
};

QT_END_NAMESPACE

#endif
