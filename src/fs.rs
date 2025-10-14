use std::{fs, io, path::Path};

pub trait FileSystem {
    fn read_to_string(&self, path: &Path) -> io::Result<String>;
    fn write_string(&self, path: &Path, content: &str) -> io::Result<()>;
}

#[non_exhaustive]
#[derive(Debug, Clone, Copy)]
pub struct RealFs;

impl FileSystem for RealFs {
    #[inline]
    fn read_to_string(&self, path: &Path) -> io::Result<String> {
        fs::read_to_string(path)
    }

    #[inline]
    fn write_string(&self, path: &Path, content: &str) -> io::Result<()> {
        fs::write(path, content)
    }
}

#[cfg(test)]
pub mod mock {
    use std::{
        collections::HashMap,
        io::{self, ErrorKind},
        path::{Path, PathBuf},
        sync::Mutex,
    };

    use crate::fs::FileSystem;

    #[derive(Debug, Default)]
    pub struct InMemoryFs {
        files: Mutex<HashMap<PathBuf, String>>,
    }

    impl InMemoryFs {
        pub fn new() -> Self {
            Self::default()
        }
    }

    impl FileSystem for InMemoryFs {
        fn read_to_string(&self, path: &Path) -> io::Result<String> {
            let files = self.files.lock().unwrap();

            files.get(path).cloned().ok_or_else(|| {
                io::Error::new(ErrorKind::NotFound, "file not found in memory")
            })
        }

        fn write_string(&self, path: &Path, content: &str) -> io::Result<()> {
            let mut files = self.files.lock().unwrap();
            let _old_content =
                files.insert(path.to_path_buf(), content.to_string());

            Ok(())
        }
    }
}

#[cfg(test)]
mod tests {
    use std::{io::ErrorKind, path::Path};

    use crate::fs::{FileSystem, RealFs, mock::InMemoryFs};

    #[test]
    fn in_memory_fs_read_string_succeeds_for_existing_path() {
        let fs = InMemoryFs::new();
        let path = Path::new("/file.txt");
        let content = "hello";
        fs.write_string(path, content).unwrap();

        let result = fs.read_to_string(path);

        assert_eq!(result.unwrap(), content);
    }

    #[test]
    fn in_memory_fs_read_string_fails_for_nonexistent_path() {
        let fs = InMemoryFs::new();
        let path = Path::new("/nonexistent.txt");

        let result = fs.read_to_string(path);

        assert!(result.is_err());
        assert_eq!(result.unwrap_err().kind(), ErrorKind::NotFound);
    }

    #[test]
    fn in_memory_fs_write_string_makes_content_readable() {
        let fs = InMemoryFs::new();
        let path = Path::new("/new_file.txt");
        let content = "new content";

        let write_result = fs.write_string(path, content);

        assert!(write_result.is_ok());
        assert_eq!(fs.read_to_string(path).unwrap(), content);
    }

    #[test]
    fn real_fs_read_string_succeeds_for_existing_file() {
        let temp_dir = tempfile::tempdir().unwrap();
        let file_path = temp_dir.path().join("real_file.txt");
        let content = "hello from a real file";
        std::fs::write(&file_path, content).unwrap();
        let fs = RealFs;

        let result = fs.read_to_string(&file_path);

        assert_eq!(result.unwrap(), content);
    }

    #[test]
    fn real_fs_read_string_fails_for_nonexistent_path() {
        let temp_dir = tempfile::tempdir().unwrap();
        let file_path = temp_dir.path().join("nonexistent.txt");
        let fs = RealFs;

        let result = fs.read_to_string(&file_path);

        assert!(result.is_err());
        assert_eq!(result.unwrap_err().kind(), ErrorKind::NotFound);
    }

    #[test]
    fn real_fs_write_string_creates_file_with_content() {
        let temp_dir = tempfile::tempdir().unwrap();
        let file_path = temp_dir.path().join("new_real_file.txt");
        let content = "new real content";
        let fs = RealFs;

        let write_result = fs.write_string(&file_path, content);

        assert!(write_result.is_ok());

        let file_content_on_disk = std::fs::read_to_string(&file_path).unwrap();
        assert_eq!(file_content_on_disk, content);
    }
}
