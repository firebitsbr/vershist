% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/tomcat.R
\name{tomcat_version_history}
\alias{tomcat_version_history}
\title{Retrieve Apache Tomcat Version Release History}
\usage{
tomcat_version_history()
}
\description{
Scrapes \url{https://tomcat.apache.org/oldnews-2010.html} to build a data frame of
openresty version release numbers and dates with pesudo-semantic version
strings parsed and separate fields added. The data frame is also arranged in
order from lowest version to latest version and the \code{vers} column is an
ordered factor.
}
\note{
alpha, beta and RCs are excluded. File an issue if these are needed.
}
