import unittest
import torch
from scipy.spatial.distance import cosine
from Metaphor import *


# in Metaphor so no need to call it in this file :}
"""
def compare_with_enrolled_users(test_embedding, enrolled_users, threshold=0.75):
    """
   # Compares a test embedding with a set of enrolled users' embeddings to determine the closest match.
   # Returns an access message based on the highest similarity.
"""
    if not enrolled_users:
        return "Access Denied: No matching user found."

    best_match, highest_similarity = None, 0

    for user_name, user_embedding in enrolled_users.items():
        similarity = 1 - cosine(test_embedding.numpy(), user_embedding.numpy())
        print(f"Similarity with {user_name}: {similarity:.4f}")  # Debugging line

        # Update best match if this user has a higher similarity
        if similarity > highest_similarity:
            highest_similarity = similarity
            best_match = user_name

    # Grant access if highest similarity meets or exceeds the threshold
    if highest_similarity >= threshold:
        return f"Access Granted: Voice matches with {best_match}!"
    else:
        return "Access Denied: No matching user found."
    
"""


# Unit testing class
class TestCompareWithEnrolledUsers(unittest.TestCase):
    def setUp(self):
        """Set up synthetic embeddings for testing."""
        self.enrolled_users = {
            "User1": torch.tensor([0.1, 0.2, 0.3, 0.4]),  # Mohammed
            "User2": torch.tensor([0.5, 0.6, 0.7, 0.8]),  # Farah
            "User3": torch.tensor([0.9, 0.95, 0.85, 0.8]),  # Hussin
        }

        self.test_embedding_match = torch.tensor([0.45, 0.55, 0.65, 0.75])  # Close to User2
        self.test_embedding_no_match = torch.tensor([0.0,0.0, 0.0, 0.0])  # No match
        self.test_embedding_best_match = torch.tensor([0.9, 0.9, 0.85, 0.8])  # Close to User3
        self.empty_enrolled_users = {}


#==============================================================================================================================#

    def log_success(self, test_name):
        """Custom function to print success message with test name."""
        print(f"{test_name} passed successfully!\n ")
        print("==============================================================")

    def test_correct_match(self):
        """Test that a correct match is identified."""
        result = compare_with_enrolled_users(self.test_embedding_match, self.enrolled_users, threshold=0.65)
        self.assertEqual(result, "Access Granted: Voice matches with User2!")
        self.log_success("test_correct_match")

    def test_no_match(self):
        """Test that no match is found for a low-similarity embedding."""
        result = compare_with_enrolled_users(self.test_embedding_no_match, self.enrolled_users, threshold=0.95)
        self.assertEqual(result, "Access Denied: No matching user found.")
        self.log_success("test_no_match")


    def test_custom_threshold(self):
        """Test that the threshold works as expected."""
        result = compare_with_enrolled_users(self.test_embedding_match, self.enrolled_users, threshold=0.955)
        self.assertEqual(result, "Access Denied: No matching user found.")
        self.log_success("test_custom_threshold")

        def test_empty_enrolled_users(self):
            result = compare_with_enrolled_users(self.test_embedding_match, self.empty_enrolled_users)
            self.assertEqual(result, "Access Denied: No matching user found.")
            self.log_success("test_empty_enrolled_users")



#==========================================================================================================================#

if __name__ == "__main__":
    unittest.main()
