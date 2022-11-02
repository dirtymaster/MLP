#include <gtest/gtest.h>
#include "Controller/controller.h"

TEST(Test, graph) {
    s21::Controller c;
    c.InitNetwork({784, 50, 50, 26}, 0.1, true);
    SUCCEED();
    c.Train(1, "../resources/emnist-letters-train.csv", false, 0, 0);
    c.Train(1, "../resources/emnist-letters-train.csv", false, 0, 0);
    EXPECT_TRUE(c.GetMseForTraining().size() > 88000);
    EXPECT_TRUE(c.GetTimeSpentOnTraining() > 1);
    EXPECT_TRUE(c.SaveWeights("../resources/test.txt"));
    // c.InitNetwork({0}, 0, true);
    EXPECT_TRUE(c.LoadWeights("../resources/test.txt"));
    c.Test(1, "../resources/emnist-letters-test.csv");
    EXPECT_TRUE(c.GetAverageAccuracy() > 0);
    EXPECT_TRUE(c.GetPrecisions().size() == 26);
    EXPECT_TRUE(c.GetRecalls().size() == 26);
    EXPECT_TRUE(c.GetFMeasures().size() == 26);
    EXPECT_TRUE(c.GetTimeSpentOnTesting() > 1);
    EXPECT_TRUE(c.GetMseForTesting().size() > 14000);
    EXPECT_TRUE(c.GetIsTrained());
}

TEST(Test, matrix) {
    s21::Controller c;
    c.InitNetwork({784, 100, 26}, 0.1, false);
    SUCCEED();
    c.Train(1, "../resources/emnist-letters-train.csv", false, 0, 0);
    c.Train(1, "../resources/emnist-letters-train.csv", false, 0, 0);
    EXPECT_TRUE(c.GetMseForTraining().size() > 88000);
    EXPECT_TRUE(c.GetTimeSpentOnTraining() > 10);
    EXPECT_TRUE(c.SaveWeights("../resources/test.txt"));
    // c.InitNetwork({0}, 0, false);
    EXPECT_TRUE(c.LoadWeights("../resources/test.txt"));
    c.Test(1, "../resources/emnist-letters-test.csv");
    EXPECT_TRUE(c.GetAverageAccuracy() > 0);
    EXPECT_TRUE(c.GetPrecisions().size() == 26);
    EXPECT_TRUE(c.GetRecalls().size() == 26);
    EXPECT_TRUE(c.GetFMeasures().size() == 26);
    EXPECT_TRUE(c.GetTimeSpentOnTesting() > 1);
    EXPECT_TRUE(c.GetMseForTesting().size() > 14000);
    EXPECT_TRUE(c.GetIsTrained());
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
