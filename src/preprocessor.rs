use rule::{InvalidRegex, Rule};
use thiserror::Error;

pub mod rule;

#[non_exhaustive]
#[derive(Debug, Error)]
pub enum ParseError<'src> {
    #[error("{0}")]
    InvalidRegex(InvalidRegex<'src>),
    #[error("malformed rule on line {line_number}: '{line_content}'")]
    MalformedRule {
        line_number: usize,
        line_content: &'src str,
    },
}

#[inline]
pub fn parse_rules(content: &str) -> Result<Vec<Rule<'_>>, ParseError<'_>> {
    content
        .lines()
        .enumerate()
        .map(|(i, line)| (i, line.trim()))
        .filter(|&(_, line)| !line.is_empty())
        .map(|(i, line)| {
            if let Some((pattern, replacement)) = line.rsplit_once(',') {
                Rule::new(pattern, replacement)
                    .map_err(ParseError::InvalidRegex)
            } else {
                Err(ParseError::MalformedRule {
                    line_number: i + 1,
                    line_content: line,
                })
            }
        })
        .collect()
}
