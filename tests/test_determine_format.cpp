// SPDX-License-Identifier: BSD-3-Clause
// Copyright 2022 Shannon Booth <shannon.ml.booth@gmail.com>

#include <gtest/gtest.h>
#include <istream>
#include <patch/hunk.h>
#include <patch/parser.h>
#include <patch/patch.h>

TEST(DetermineFormat, Unified)
{
    std::stringstream patch_file(R"(--- a.cpp	2022-03-20 12:42:14.665007336 +1300
+++ b.cpp	2022-03-20 12:42:20.772998512 +1300
@@ -1,3 +1,4 @@
 int main()
 {
+	return 1;
 }
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info, -1);
    EXPECT_EQ(patch.format, Patch::Format::Unified);

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);
    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a unified diff to me...
The text leading up to this was:
--------------------------
|--- a.cpp	2022-03-20 12:42:14.665007336 +1300
|+++ b.cpp	2022-03-20 12:42:20.772998512 +1300
--------------------------
)");
}

TEST(DetermineFormat, Git)
{
    std::stringstream patch_file(R"(diff --git a/b.cpp b/b.cpp
index 5047a34..a46866d 100644
--- a/b.cpp
+++ b/b.cpp
@@ -1,3 +1,4 @@
 int main()
 {
+       return 0;
 }
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info, -1);
    EXPECT_EQ(patch.format, Patch::Format::Unified);

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);
    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a unified diff to me...
The text leading up to this was:
--------------------------
|diff --git a/b.cpp b/b.cpp
|index 5047a34..a46866d 100644
|--- a/b.cpp
|+++ b/b.cpp
--------------------------
)");
}

TEST(DetermineFormat, GitExtendedRenameNoHunk)
{
    std::stringstream patch_file(R"(diff --git a/new_file b/another_new
similarity index 100%
rename from new_file
rename to another_new
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info, -1);
    EXPECT_EQ(patch.format, Patch::Format::Unified);
    EXPECT_EQ(patch.operation, Patch::Operation::Rename);
    EXPECT_EQ(patch.old_file_path, "new_file");
    EXPECT_EQ(patch.new_file_path, "another_new");

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);

    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a unified diff to me...
The text leading up to this was:
--------------------------
|diff --git a/new_file b/another_new
|similarity index 100%
|rename from new_file
|rename to another_new
--------------------------
)");
}

TEST(DetermineFormat, GitExtendedRenameWithHunk)
{
    std::stringstream patch_file(R"(diff --git a/file b/test
similarity index 87%
rename from a/b/c/d/thing
rename to a/b/c/d/e/test
index 71ac1b5..fc3102f 100644
--- a/thing
+++ b/test
@@ -2,7 +2,6 @@ a
 b
 c
 d
-e
 f
 g
 h
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info, -1);
    EXPECT_EQ(patch.format, Patch::Format::Unified);
    EXPECT_EQ(patch.operation, Patch::Operation::Rename);
    EXPECT_EQ(patch.old_file_path, "thing");
    EXPECT_EQ(patch.new_file_path, "test");

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);

    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a unified diff to me...
The text leading up to this was:
--------------------------
|diff --git a/file b/test
|similarity index 87%
|rename from a/b/c/d/thing
|rename to a/b/c/d/e/test
|index 71ac1b5..fc3102f 100644
|--- a/thing
|+++ b/test
--------------------------
)");
}

TEST(DetermineFormat, Context)
{
    std::stringstream patch_file(R"(*** a.cpp	2022-04-03 18:41:54.611014944 +1200
--- c.cpp	2022-04-03 18:42:00.850801875 +1200
***************
*** 1,3 ****
--- 1,4 ----
  int main()
  {
+     return 0;
  }
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info);
    EXPECT_EQ(patch.format, Patch::Format::Context);

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);
    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a context diff to me...
The text leading up to this was:
--------------------------
|*** a.cpp	2022-04-03 18:41:54.611014944 +1200
|--- c.cpp	2022-04-03 18:42:00.850801875 +1200
--------------------------
)");
}

TEST(DetermineFormat, ContextWithUnifiedRangeInHeader)
{
    std::stringstream patch_file(R"(
Some text
@@ -1,29 +0,0 @@

*** a.cpp	2022-04-03 18:41:54.611014944 +1200
--- c.cpp	2022-04-03 18:42:00.850801875 +1200
***************
*** 1,3 ****
--- 1,4 ----
  int main()
  {
+     return 0;
  }
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info);
    EXPECT_EQ(patch.format, Patch::Format::Context);

    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);

    std::cout << "----------------------------------------------------------------\n";
    std::cout << output.str() << "\n";
    std::cout << "----------------------------------------------------------------\n";

    EXPECT_EQ(output.str(),
        R"(Hmm...  Looks like a context diff to me...
The text leading up to this was:
--------------------------
|
|Some text
|@@ -1,29 +0,0 @@
|
|*** a.cpp	2022-04-03 18:41:54.611014944 +1200
|--- c.cpp	2022-04-03 18:42:00.850801875 +1200
--------------------------
)");
}

TEST(DetermineFormat, Normal)
{
    std::stringstream patch_file(R"(2a3
> 	return 0;
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info);
    EXPECT_EQ(patch.format, Patch::Format::Normal);
    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);
    EXPECT_EQ(output.str(), "Hmm...  Looks like a normal diff to me...\n");
}

TEST(DetermineFormat, NormalWithFromAndToFileLines)
{
    std::stringstream patch_file(R"(Index: thing
+++ a.cpp
--- b.cpp
*** c.cpp
2a3
> 	return 0;
)");

    Patch::Patch patch;
    Patch::PatchHeaderInfo info;
    Patch::parse_patch_header(patch, patch_file, info);
    EXPECT_EQ(patch.format, Patch::Format::Normal);
    std::stringstream output;
    Patch::print_header_info(patch_file, info, output);
    EXPECT_EQ(output.str(), R"(Hmm...  Looks like a normal diff to me...
The text leading up to this was:
--------------------------
|Index: thing
|+++ a.cpp
|--- b.cpp
|*** c.cpp
--------------------------
)");
    EXPECT_EQ(patch.index_file_path, "thing");
    EXPECT_EQ(patch.new_file_path, "");
    EXPECT_EQ(patch.old_file_path, "");
}

TEST(DetermineFormat, LooksLikeNormalCommand)
{
    Patch::Hunk hunk;

    // Possibilities given in POSIX diff utility guidelines, with %d substituted for a random integer.
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "1a2"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "1a23,3"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "12d2"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "1,2d3"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "10c20"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "1,2c31"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "9c2,3"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "1c5,93"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "18c2,3"));
    EXPECT_TRUE(Patch::parse_normal_range(hunk, "5,7c8,10"));

    // Can only find two commas for a change command.
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "5,7d8,10"));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "5,7a8,10"));

    // Some other invalid combinations.
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "> Some normal addition"));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "5,7c8,10 "));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, " 5,7c8,10 "));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "5.7c8,10 "));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "1,2x3"));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "1a2."));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, "1a~2'"));
    EXPECT_FALSE(Patch::parse_normal_range(hunk, ""));
}

TEST(DetermineFormat, LooksLikeUnifiedRange)
{
    Patch::Hunk hunk;
    EXPECT_TRUE(Patch::parse_unified_range(hunk, "@@ -1,3 +1,4 @@"));
    EXPECT_EQ(hunk.old_file_range.start_line, 1);
    EXPECT_EQ(hunk.old_file_range.number_of_lines, 3);
    EXPECT_EQ(hunk.new_file_range.start_line, 1);
    EXPECT_EQ(hunk.new_file_range.number_of_lines, 4);

    EXPECT_TRUE(Patch::parse_unified_range(hunk, "@@ -2,0 +3 @@"));
    EXPECT_EQ(hunk.old_file_range.start_line, 2);
    EXPECT_EQ(hunk.old_file_range.number_of_lines, 0);
    EXPECT_EQ(hunk.new_file_range.start_line, 3);
    EXPECT_EQ(hunk.new_file_range.number_of_lines, 1);

    EXPECT_TRUE(Patch::parse_unified_range(hunk, "@@ -3 +2,0 @@"));
    EXPECT_EQ(hunk.old_file_range.start_line, 3);
    EXPECT_EQ(hunk.old_file_range.number_of_lines, 1);
    EXPECT_EQ(hunk.new_file_range.start_line, 2);
    EXPECT_EQ(hunk.new_file_range.number_of_lines, 0);

    EXPECT_FALSE(Patch::parse_unified_range(hunk, "@@ -3 +2,0 @"));
    EXPECT_FALSE(Patch::parse_unified_range(hunk, "@@ -3 +2.0 @@"));
    EXPECT_FALSE(Patch::parse_unified_range(hunk, "@@ -5,1a +9,8 @@"));
}

TEST(DetermineFormat, StringToUint32)
{
    Patch::LineNumber output = 0;
    EXPECT_TRUE((Patch::string_to_line_number("2", output)));
    EXPECT_EQ(output, 2);

    EXPECT_TRUE((Patch::string_to_line_number("100", output)));
    EXPECT_EQ(output, 100);

    EXPECT_TRUE((Patch::string_to_line_number("9223372036854775807", output)));
    EXPECT_EQ(output, 9223372036854775807);

    // Overflow
    EXPECT_FALSE((Patch::string_to_line_number("9223372036854775808", output)));

    // Empty!?
    EXPECT_FALSE((Patch::string_to_line_number("", output)));

    // Bad char
    EXPECT_FALSE((Patch::string_to_line_number("1a2", output)));
    EXPECT_FALSE((Patch::string_to_line_number("a1", output)));
}
