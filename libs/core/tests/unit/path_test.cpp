#include "wendfyr/domain/path.hpp"

#include <gtest/gtest.h>

namespace wendfyr::tests
{
    using domain::Path;

    TEST(PathTest, DefaultConstructsEmpty)
    {
        Path path{};
        EXPECT_TRUE(path.empty());
        EXPECT_EQ(path.string(), "");
    }

    TEST(PathTest, ConstructsFromString)
    {
        Path path{"/home/user"};
        EXPECT_EQ(path.string(), "/home/user");
        EXPECT_FALSE(path.empty());
    }

    TEST(PathTest, ConstructFromCString)
    {
        Path path = "/home/user";
        EXPECT_EQ(path.string(), "/home/user");
        EXPECT_FALSE(path.empty());
    }

    TEST(PathTest, FilenameOfFile)
    {
        Path path{"/home/user/file.pdf"};
        EXPECT_EQ(path.filename().string(), "file.pdf");
    }

    TEST(PathTest, FilenameOfDirectory)
    {
        Path path{"/home/user/directory"};
        EXPECT_EQ(path.filename().string(), "directory");
    }

    TEST(PathTest, FilenameOfRoot)
    {
        Path path{"/"};
        EXPECT_EQ(path.filename().string(), "");
    }

    TEST(PathTest, FilenameWithTrailingSlash)
    {
        Path path{"/home/user/file.pdf/"};
        EXPECT_EQ(path.filename().string(), "file.pdf");
    }

    TEST(PathTest, FilenameOfRelative)
    {
        Path path{"file.pdf"};
        EXPECT_EQ(path.filename().string(), "file.pdf");
    }

    TEST(PathTest, FilenameOfEmpty)
    {
        Path path;
        EXPECT_EQ(path.filename().string(), "");
    }

    TEST(PathTest, StemOfNormalFile)
    {
        Path path{"/home/user/file.pdf"};
        EXPECT_EQ(path.stem().string(), "file");
    }

    TEST(PathTest, ExtensionOfNormalFile)
    {
        Path path{"/home/user/file.pdf"};
        EXPECT_EQ(path.extension().string(), ".pdf");
    }

    TEST(PathTest, StemOfHiddenFile)
    {
        Path path{"/home/user/.bashrc"};
        EXPECT_EQ(path.stem().string(), ".bashrc");
        EXPECT_EQ(path.extension().string(), "");
    }

    TEST(PathTest, StemOfNoExtension)
    {
        Path path{"/home/CMake"};
        EXPECT_EQ(path.stem().string(), "CMake");
        EXPECT_EQ(path.extension().string(), "");
    }

    TEST(PathTest, StemOfDoubleExtension)
    {
        Path path{"/home/arc.tar.gz"};
        EXPECT_EQ(path.stem().string(), "arc.tar");
        EXPECT_EQ(path.extension().string(), ".gz");
    }

    TEST(PathTest, ParentOfFile)
    {
        Path path{"/home/user/file.pdf"};
        EXPECT_EQ(path.parentPath().string(), "/home/user");
    }

    TEST(PathTest, ParentOfDirectory)
    {
        Path path{"/home/user"};
        EXPECT_EQ(path.parentPath().string(), "/home");
    }

    TEST(PathTest, ParentOfRoot)
    {
        Path path{"/"};
        EXPECT_EQ(path.parentPath().string(), "/");
    }

    TEST(PathTest, ParentOfTrailingSlash)
    {
        Path path{"/home/user/"};
        EXPECT_EQ(path.parentPath().string(), "/home");
    }
    TEST(PathTest, ParentOfRelative)
    {
        Path path{"file.pdf"};
        EXPECT_TRUE(path.parentPath().empty());
    }

    TEST(PathTest, ParentOfSingleDir)
    {
        Path path{"/home"};
        EXPECT_EQ(path.parentPath().string(), "/");
    }

    TEST(PathTest, AbsolutePathIsAbsolute)
    {
        Path path{"/home/user/file.pdf"};
        EXPECT_TRUE(path.isAbsolute());
    }

    TEST(PathTest, RelativePathIsNotAbsolute)
    {
        Path path{"home/user"};
        EXPECT_FALSE(path.isAbsolute());
    }

    TEST(PathTest, EmptyPathIsNotAbsolute)
    {
        Path path;
        EXPECT_FALSE(path.isAbsolute());
    }

    TEST(PathTest, JoinTwoPaths)
    {
        Path path{Path{"/home"} / "user"};
        EXPECT_EQ(path.string(), "/home/user");
    }

    TEST(PathTest, JoinMultiplePaths)
    {
        Path path{Path{"/home"} / "user" / "documents" / "file.pdf"};
        EXPECT_EQ(path.string(), "/home/user/documents/file.pdf");
    }

    TEST(PathTest, JoinWithTrailingSlash)
    {
        Path path{Path{"/home/"} / "user"};
        EXPECT_EQ(path.string(), "/home/user");
    }

    TEST(PathTest, JoinEmptyLeft)
    {
        Path path{Path{} / "user"};
        EXPECT_EQ(path.string(), "user");
    }

    TEST(PathTest, JoinEmptyRight)
    {
        Path path{Path{"/home"} / ""};
        EXPECT_EQ(path.string(), "/home");
    }

    TEST(PathTest, JoinAbsoluteRightReplaces)
    {
        Path path{Path{"/home/user"} / "/etc/config"};
        EXPECT_EQ(path.string(), "/etc/config");
    }

    TEST(PathTest, CompoundAssignment)
    {
        Path path{"/home"};
        path /= "user";
        path /= "file.pdf";
        EXPECT_EQ(path.string(), "/home/user/file.pdf");
    }

    TEST(PathTest, EqualPathsAreEqual)
    {
        EXPECT_EQ(Path{"/home"}, Path{"/home"});
    }

    TEST(PathTest, DifferentPathsAreNotEqual)
    {
        EXPECT_NE(Path{"/home"}, Path{"/temp"});
    }

    TEST(PathTest, LessThenOrdering)
    {
        EXPECT_LT(Path{"/a"}, Path{"/b"});
    }
};  // namespace wendfyr::tests
