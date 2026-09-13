import tempfile
import unittest
from pathlib import Path

from scripts.validate_repo import validate_repository


HARDWARE = ("esp32-p4", "esp32-s3", "raspberry-pi-4", "arduino-uno")


class RepositoryValidatorTests(unittest.TestCase):
    def make_repository(self) -> Path:
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        root = Path(temp.name)
        (root / "README.md").write_text("# QHardware\n", encoding="utf-8")
        (root / "LICENSE").write_text("MIT\n", encoding="utf-8")
        (root / ".gitignore").write_text("build/\n", encoding="utf-8")
        docs = root / "docs"
        docs.mkdir()
        (docs / "lab-conventions.md").write_text("# Lab conventions\n", encoding="utf-8")
        (docs / "verification-format.md").write_text("# Verification\n", encoding="utf-8")
        for hardware in HARDWARE:
            directory = root / hardware
            directory.mkdir()
            (directory / "README.md").write_text(f"# {hardware}\n", encoding="utf-8")
            (directory / "exercises").mkdir()
        return root

    def add_exercise(self, root: Path, state: str = "DOCUMENTED") -> Path:
        exercise = root / "esp32-p4" / "exercises" / "demo"
        verification = exercise / "verification"
        verification.mkdir(parents=True)
        (exercise / "README.md").write_text(
            f"# Demo\n\nVerification state: `{state}`\n", encoding="utf-8"
        )
        (verification / "method.md").write_text("# Method\n", encoding="utf-8")
        (verification / "expected-output.txt").write_text("ready\n", encoding="utf-8")
        (verification / "actual-output.txt").write_text("not run\n", encoding="utf-8")
        (verification / "checksums.sha256").write_text(
            "# No distributable artifacts.\n", encoding="utf-8"
        )
        return exercise

    def test_valid_minimal_repository(self) -> None:
        root = self.make_repository()
        self.add_exercise(root)
        self.assertEqual(validate_repository(root), [])

    def test_missing_verification_method_is_reported(self) -> None:
        root = self.make_repository()
        exercise = self.add_exercise(root)
        (exercise / "verification" / "method.md").unlink()
        errors = validate_repository(root)
        self.assertTrue(any("verification/method.md" in error for error in errors))

    def test_missing_esp32_s3_readme_is_reported(self) -> None:
        root = self.make_repository()
        (root / "esp32-s3" / "README.md").unlink()
        errors = validate_repository(root)
        self.assertIn("missing hardware README: esp32-s3/README.md", errors)

    def test_invalid_evidence_state_is_reported(self) -> None:
        root = self.make_repository()
        self.add_exercise(root, state="TESTED")
        errors = validate_repository(root)
        self.assertTrue(any("evidence state" in error for error in errors))

    def test_windows_user_path_is_reported(self) -> None:
        root = self.make_repository()
        self.add_exercise(root)
        (root / "notes.md").write_text(
            "SDK: C:\\Users\\alice\\esp-idf\n", encoding="utf-8"
        )
        errors = validate_repository(root)
        self.assertTrue(any("absolute user path" in error for error in errors))

    def test_unix_home_path_is_reported(self) -> None:
        root = self.make_repository()
        self.add_exercise(root)
        (root / "notes.md").write_text("SDK: /home/alice/esp-idf\n", encoding="utf-8")
        errors = validate_repository(root)
        self.assertTrue(any("absolute user path" in error for error in errors))

    def test_credential_assignment_is_reported(self) -> None:
        root = self.make_repository()
        self.add_exercise(root)
        (root / "settings.conf").write_text(
            'WIFI_PASSWORD="real-password"\n', encoding="utf-8"
        )
        errors = validate_repository(root)
        self.assertTrue(any("credential-like assignment" in error for error in errors))


if __name__ == "__main__":
    unittest.main()
