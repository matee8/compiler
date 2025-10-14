use regex::Regex;
use thiserror::Error;

#[non_exhaustive]
#[derive(Debug, Error)]
#[error("invalid regex pattern '{pattern}': {source}")]
pub struct InvalidRegex<'pattern> {
    pattern: &'pattern str,
    source: regex::Error,
}

#[non_exhaustive]
#[derive(Debug)]
pub struct Rule<'src> {
    pub pattern: &'src str,
    pub replacement: &'src str,
    pub compiled_regex: Regex,
}

impl<'src> Rule<'src> {
    #[inline]
    pub fn new(
        pattern: &'src str,
        replacement: &'src str,
    ) -> Result<Self, InvalidRegex<'src>> {
        let compiled_regex =
            Regex::new(pattern).map_err(|err| InvalidRegex {
                pattern,
                source: err,
            })?;

        Ok(Self {
            pattern,
            replacement,
            compiled_regex,
        })
    }
}
