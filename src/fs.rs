use std::{fs, io, path::Path};

pub trait FileSystem {
    fn read_to_string(&self, path: &Path) -> io::Result<String>;
    fn write_string(&self, path: &Path, content: &str) -> io::Result<()>;
}

#[non_exhaustive]
#[derive(Clone, Copy, Debug)]
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
