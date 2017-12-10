#include "tagtype/Flags.h"

#include <QtTest>

class flagsTest : public QObject {
    Q_OBJECT

private slots:
    void test__tagtype_Flags__all() {
        using namespace meta;
        auto flags = tagtype::Flags<char, int, float>::setAll();
        auto anded = flags & tagtype::Flag<int>{};
        QCOMPARE(anded, tagtype::Flag<int>{});
    }
};

QTEST_APPLESS_MAIN(flagsTest)

#include "flagstest.moc"
