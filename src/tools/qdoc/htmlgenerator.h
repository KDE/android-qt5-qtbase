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
  htmlgenerator.h
*/

#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include <QMap>
#include <QRegExp>
#include <QXmlStreamWriter>
#include "codemarker.h"
#include "config.h"
#include "generator.h"

QT_BEGIN_NAMESPACE

class HelpProjectWriter;

class HtmlGenerator : public Generator
{
public:
    enum SinceType {
        Namespace,
        Class,
        MemberFunction,
        NamespaceFunction,
        GlobalFunction,
        Macro,
        Enum,
        Typedef,
        Property,
        Variable,
        QmlClass,
        QmlProperty,
        QmlSignal,
        QmlSignalHandler,
        QmlMethod,
        LastSinceType
    };

public:
    HtmlGenerator();
    ~HtmlGenerator();

    virtual void initializeGenerator(const Config& config);
    virtual void terminateGenerator();
    virtual QString format();
    virtual void generateTree(Tree *tree);
    void generateCollisionPages();
    void generateManifestFiles();

    QString protectEnc(const QString &string);
    static QString protect(const QString &string, const QString &encoding = "ISO-8859-1");
    static QString cleanRef(const QString& ref);
    static QString sinceTitle(int i) { return sinceTitles[i]; }

protected:
    virtual void startText(const Node *relative, CodeMarker *marker);
    virtual int generateAtom(const Atom *atom,
                             const Node *relative,
                             CodeMarker *marker);
    virtual void generateClassLikeNode(InnerNode* inner, CodeMarker* marker);
    virtual void generateDocNode(DocNode* dn, CodeMarker* marker);
    virtual QString fileExtension() const;
    virtual QString refForNode(const Node *node);
    virtual QString linkForNode(const Node *node, const Node *relative);
    virtual QString refForAtom(Atom *atom, const Node *node);

    void generateManifestFile(QString manifest, QString element);

private:
    enum SubTitleSize { SmallSubTitle, LargeSubTitle };
    enum ExtractionMarkType {
        BriefMark,
        DetailedDescriptionMark,
        MemberMark,
        EndMark
    };

    const QPair<QString,QString> anchorForNode(const Node *node);
    const Node *findNodeForTarget(const QString &target,
                                  const Node *relative,
                                  CodeMarker *marker,
                                  const Atom *atom = 0);
    void generateBreadCrumbs(const QString& title,
                             const Node *node,
                             CodeMarker *marker);
    void generateHeader(const QString& title,
                        const Node *node = 0,
                        CodeMarker *marker = 0);
    void generateTitle(const QString& title,
                       const Text &subTitle,
                       SubTitleSize subTitleSize,
                       const Node *relative,
                       CodeMarker *marker);
    void generateFooter(const Node *node = 0);
    void generateBrief(const Node *node,
                       CodeMarker *marker,
                       const Node *relative = 0);
    void generateIncludes(const InnerNode *inner, CodeMarker *marker);
    void generateTableOfContents(const Node *node,
                                 CodeMarker *marker,
                                 QList<Section>* sections = 0);
    QString generateListOfAllMemberFile(const InnerNode *inner,
                                        CodeMarker *marker);
    QString generateAllQmlMembersFile(const QmlClassNode* qml_cn,
                                      CodeMarker* marker);
    QString generateLowStatusMemberFile(const InnerNode *inner,
                                        CodeMarker *marker,
                                        CodeMarker::Status status);
    void generateClassHierarchy(const Node *relative,
                                CodeMarker *marker,
                                const NodeMap &classMap);
    void generateAnnotatedList(const Node *relative,
                               CodeMarker *marker,
                               const NodeMap &nodeMap,
                               bool allOdd = false);
    void generateCompactList(const Node *relative,
                             CodeMarker *marker,
                             const NodeMap &classMap,
                             bool includeAlphabet,
                             QString commonPrefix = QString());
    void generateFunctionIndex(const Node *relative, CodeMarker *marker);
    void generateLegaleseList(const Node *relative, CodeMarker *marker);
    void generateOverviewList(const Node *relative, CodeMarker *marker);
    void generateSectionList(const Section& section,
                             const Node *relative,
                             CodeMarker *marker,
                             CodeMarker::SynopsisStyle style);
    void generateQmlSummary(const Section& section,
                            const Node *relative,
                            CodeMarker *marker);
    void generateQmlItem(const Node *node,
                         const Node *relative,
                         CodeMarker *marker,
                         bool summary);
    void generateDetailedQmlMember(Node *node,
                                   const InnerNode *relative,
                                   CodeMarker *marker);
    void generateQmlInherits(const QmlClassNode* qcn, CodeMarker* marker);
    void generateQmlInstantiates(QmlClassNode* qcn, CodeMarker* marker);
    void generateInstantiatedBy(ClassNode* cn, CodeMarker* marker);

    void generateSection(const NodeList& nl,
                         const Node *relative,
                         CodeMarker *marker,
                         CodeMarker::SynopsisStyle style);
    void generateSynopsis(const Node *node,
                          const Node *relative,
                          CodeMarker *marker,
                          CodeMarker::SynopsisStyle style,
                          bool alignNames = false,
                          const QString* prefix = 0);
    void generateSectionInheritedList(const Section& section,
                                      const Node *relative,
                                      CodeMarker *marker);
    QString highlightedCode(const QString& markedCode,
                            CodeMarker* marker,
                            const Node* relative,
                            bool alignNames = false,
                            const Node* self = 0);

    void generateFullName(const Node *apparentNode,
                          const Node *relative,
                          CodeMarker *marker,
                          const Node *actualNode = 0);
    void generateDetailedMember(const Node *node,
                                const InnerNode *relative,
                                CodeMarker *marker);
    void generateLink(const Atom *atom,
                      const Node *relative,
                      CodeMarker *marker);
    void generateStatus(const Node *node, CodeMarker *marker);

    QString registerRef(const QString& ref);
    virtual QString fileBase(const Node *node) const;
    QString fileName(const Node *node);
    void findAllClasses(const InnerNode *node);
    void findAllFunctions(const InnerNode *node);
    void findAllLegaleseTexts(const InnerNode *node);
    void findAllNamespaces(const InnerNode *node);
    static int hOffset(const Node *node);
    static bool isThreeColumnEnumValueTable(const Atom *atom);
    QString getLink(const Atom *atom,
                    const Node *relative,
                    CodeMarker *marker,
                    const Node** node);
    virtual void generateIndex(const QString &fileBase,
                               const QString &url,
                               const QString &title);
#ifdef GENERATE_MAC_REFS
    void generateMacRef(const Node *node, CodeMarker *marker);
#endif
    void beginLink(const QString &link,
                   const Node *node,
                   const Node *relative,
                   CodeMarker *marker);
    void endLink();
    void generateExtractionMark(const Node *node, ExtractionMarkType markType);
    void reportOrphans(const InnerNode* parent);

    void beginDitamapPage(const InnerNode* node, const QString& fileName);
    void endDitamapPage();
    void writeDitaMap(const DitaMapNode* node);
    void writeDitaRefs(const DitaRefList& ditarefs);
    QXmlStreamWriter& xmlWriter();

    QMap<QString, QString> refMap;
    int codeIndent;
    HelpProjectWriter *helpProjectWriter;
    bool inLink;
    bool inObsoleteLink;
    bool inContents;
    bool inSectionHeading;
    bool inTableHeader;
    int numTableRows;
    bool threeColumnEnumValueTable;
    QString link;
    QStringList sectionNumber;
    QRegExp funcLeftParen;
    QString style;
    QString headerScripts;
    QString headerStyles;
    QString endHeader;
    QString postHeader;
    QString postPostHeader;
    QString footer;
    QString address;
    bool pleaseGenerateMacRef;
    bool noBreadCrumbs;
    QString project;
    QString projectDescription;
    QString projectUrl;
    QString navigationLinks;
    QString manifestDir;
    QStringList stylesheets;
    QStringList customHeadElements;
    bool obsoleteLinks;
    QMap<QString, NodeMap > moduleClassMap;
    QMap<QString, NodeMap > moduleNamespaceMap;
    NodeMap nonCompatClasses;
    NodeMap mainClasses;
    NodeMap compatClasses;
    NodeMap obsoleteClasses;
    NodeMap namespaceIndex;
    NodeMap serviceClasses;
    NodeMap qmlClasses;
    QMap<QString, NodeMap > funcIndex;
    QMap<Text, const Node *> legaleseTexts;
    QStack<QXmlStreamWriter*> xmlWriterStack;
    static int id;
public:
    static bool debugging_on;
    static QString divNavTop;
};

#define HTMLGENERATOR_ADDRESS           "address"
#define HTMLGENERATOR_FOOTER            "footer"
#define HTMLGENERATOR_GENERATEMACREFS   "generatemacrefs" // ### document me
#define HTMLGENERATOR_POSTHEADER        "postheader"
#define HTMLGENERATOR_POSTPOSTHEADER    "postpostheader"
#define HTMLGENERATOR_NOBREADCRUMBS     "nobreadcrumbs"

QT_END_NAMESPACE

#endif

