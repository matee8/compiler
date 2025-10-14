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
        .filter(|&(_, line)| !line.trim().is_empty())
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

#[cfg(test)]
mod tests {
    use crate::preprocessor::{self, ParseError, rule::InvalidRegex};

    #[test]
    fn parse_rules_succeeds_with_valid_input() {
        let content = "a,b\n^start,end\n(x),(y)";

        let result = preprocessor::parse_rules(content);

        assert!(result.is_ok());
        let rules = result.unwrap();
        assert_eq!(rules.len(), 3);
        assert_eq!(rules[0].pattern, "a");
        assert_eq!(rules[0].replacement, "b");
        assert_eq!(rules[1].pattern, "^start");
        assert_eq!(rules[1].replacement, "end");
        assert_eq!(rules[2].pattern, "(x)");
        assert_eq!(rules[2].replacement, "(y)");
    }

    #[test]
    fn parse_rules_fails_on_malformed_line_without_comma() {
        let content = "a,b\nthis is a malformed line\nc,d";

        let result = preprocessor::parse_rules(content);

        assert!(result.is_err());
        match result.unwrap_err() {
            ParseError::MalformedRule {
                line_number,
                line_content,
            } => {
                assert_eq!(line_number, 2);
                assert_eq!(line_content, "this is a malformed line");
            }
            e => panic!("Expected MalformedRule error, but got {:?}", e),
        }
    }

    #[test]
    fn parse_rules_fails_on_invalid_regex_pattern() {
        let content = "a,b\n(unclosed,replacement\nc,d";

        let result = preprocessor::parse_rules(content);

        assert!(result.is_err());
        match result.unwrap_err() {
            ParseError::InvalidRegex(InvalidRegex { pattern, source: _ }) => {
                assert_eq!(pattern, "(unclosed");
            }
            e => panic!("Expected InvalidRegex error, but got {:?}", e),
        }
    }

    #[test]
    fn parse_rules_ignores_blank_lines_and_whitespace() {
        let content =
            "\n  a,b\n\n    # comment-like line,replacement\n  \t  \n c,d \n";

        let result = preprocessor::parse_rules(content);

        assert!(result.is_ok());
        let rules = result.unwrap();
        assert_eq!(rules.len(), 3);
        assert_eq!(rules[0].pattern, "  a");
        assert_eq!(rules[0].replacement, "b");
        assert_eq!(rules[1].pattern, "    # comment-like line");
        assert_eq!(rules[1].replacement, "replacement");
        assert_eq!(rules[2].pattern, " c");
        assert_eq!(rules[2].replacement, "d ");
    }
}
