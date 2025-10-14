use regex::Regex;
use thiserror::Error;

#[non_exhaustive]
#[derive(Debug, Error)]
#[error("invalid regex pattern '{pattern}': {source}")]
pub struct InvalidRegex<'pattern> {
    pub pattern: &'pattern str,
    pub source: regex::Error,
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

#[cfg(test)]
mod tests {
    use crate::preprocessor::rule::Rule;

    #[test]
    fn new_succeeds_with_valid_pattern() {
        let pattern = "^start";
        let replacement = "end";

        let result = Rule::new(pattern, replacement);

        assert!(result.is_ok());
        let rule = result.unwrap();
        assert_eq!(rule.pattern, pattern);
        assert_eq!(rule.replacement, replacement);
        assert!(rule.compiled_regex.is_match("start of line"));
    }

    #[test]
    fn new_fails_with_invalid_pattern() {
        let invalid_pattern = "(\\d+";
        let replacement = "number";

        let result = Rule::new(invalid_pattern, replacement);

        assert!(result.is_err());
        let error = result.unwrap_err();

        assert_eq!(error.pattern, invalid_pattern);
    }
}
