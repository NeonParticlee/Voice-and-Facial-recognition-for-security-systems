import unittest
from unittest.mock import patch, MagicMock
import torch
from Metaphor import VoiceRecognitionSystem, AudioPreprocessor, VoiceRecognitionConfig

class TestVoiceRecognitionSystem(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        """Print header for test suite"""
        print("\nStarting Voice Recognition System Tests")
        print("=========================\n")

    def setUp(self):
        """Initialize the voice recognition system before each test."""
        self.system = VoiceRecognitionSystem()
        self.sample_waveform = torch.randn(1, 16000)
        self.enrolled_users = {
            "Mohammed": torch.randn(1, 192),
            "Farah": torch.randn(1, 192)
        }

    def tearDown(self):
        """Print test name and status after each test."""
        test_name = self._testMethodName
        print(f"{test_name} - PASS\n=========================")

    @patch("torchaudio.load")
    def test_process_audio_file(self, mock_load):
        """Test audio preprocessing."""
        mock_waveform = torch.randn(1, 16000)
        mock_load.return_value = (mock_waveform, 44100)
        
        waveform, sr = self.system.preprocessor.process_audio_file("H4.wav")
        
        mock_load.assert_called_once_with("H4.wav")
        self.assertIsNotNone(waveform)
        self.assertEqual(sr, self.system.config.target_sr)
        self.assertEqual(waveform.dim(), 2)

    @patch("glob.glob")
    @patch("torch.stack")
    def test_enroll_user(self, mock_stack, mock_glob):
        """Test user enrollment."""
        mock_glob.return_value = ["H4.wav", "H5.wav"]
        mock_embeddings = torch.randn(2, 192)
        mock_stack.return_value = mock_embeddings
        
        with patch.object(self.system, 'get_embedding') as mock_get_embedding:
            mock_get_embedding.return_value = torch.randn(1, 192)
            result = self.system.enroll_user("Mohammed")
        
        mock_glob.assert_called_once_with(
            "Mohammed\\*.wav"
        )
        self.assertIsNotNone(result)
        self.assertEqual(result.shape[-1], 192)

    def test_verify_speaker(self):
        """Test speaker verification."""
        test_file = "M7.wav"
        
        with patch.object(self.system, 'get_embedding') as mock_get_embedding:
            mock_embedding = torch.randn(1, 192)
            mock_get_embedding.return_value = mock_embedding
            result = self.system.verify_speaker(test_file, self.enrolled_users)
        
        self.assertIsNotNone(result)
        self.assertIn('match', result)
        self.assertIn('confidence', result)
        self.assertIn('similarities', result)
        self.assertEqual(len(result['similarities']), len(self.enrolled_users))

    def test_configuration(self):
        """Test system configuration."""
        self.assertEqual(self.system.config.target_sr, 16000)
        self.assertIsNotNone(self.system.config.threshold)
        self.assertIsNotNone(self.system.config.model_path)
        self.assertTrue(hasattr(self.system, 'preprocessor'))
        self.assertTrue(hasattr(self.system, 'classifier'))

if __name__ == "__main__":
    unittest.main(verbosity=0.75)  # Reduce unittest's default output