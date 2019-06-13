# Notes on JavaFX Features

The guidelines for contributing a new features to JavaFX are referred to in the
[Before submitting a pull request](https://github.com/javafxports/openjdk-jfx/blob/develop/.github/CONTRIBUTING.md#before-submitting-a-pull-request)
section of [CONTRIBUTING.md](https://github.com/javafxports/openjdk-jfx/blob/develop/.github/CONTRIBUTING.md), which points to the
[New features / API additions](https://wiki.openjdk.java.net/display/OpenJFX/Code+Reviews#CodeReviews-NewFeaturesC.Newfeatures/APIadditions.)
section of the [OpenJFX Code Review Policies](https://wiki.openjdk.java.net/display/OpenJFX/Code+Reviews).

The note below contains some additional detail about adding new features to JavaFX. Much of this should make its way back to the
`CONTRIBUTING` or `Code Review Policies` docs.

### Adding New JavaFX Features

Adding new API means that we have to consider what it means to support that API forever; we take compatibility seriously. The main
thing is to think in terms of "stewardship" when evolving the JavaFX API. With that in mind, here are the needed steps to get a new
feature into JavaFX.

1. Discuss the proposed feature on the [openjfx-dev](mailto:openjfx-dev@openjdk.java.net) mailing list.
You should start with _why_ you think
adding the API to the core of JavaFX is a good and useful addition for multiple applications (not just your own)
and for the evolution of the JavaFX UI Toolkit. Part of this is to see whether the Project Leads and Reviewers
are generally supportive of the idea, as well as to see whether other developers have any ideas as to whether
and how it would be useful in their applications. We want to make sure that the new feature fits in with the
existing API and will move the API forward in a direction we want to go. We need to ensure that the value
proposition of the new feature justifies the investment, which goes well beyond the initial cost of developing it.
Presuming that the feature meets the cost / benefit assessment (including opportunity cost), then discussion can
proceed to the API.

2. Discuss the API needed to provide the feature. While this can't always be completely separated from its
implementation, it is the public API itself that is important to nail down and get right. While we don't currently
use the formal JEP process as is done for larger JDK features, the [JEP template](http://openjdk.java.net/jeps/2)
provides some ideas to consider when proposing an API, such as a summary of the changes, goals, motivation, testing,
dependencies, etc. A WIP pull request can be useful for illustrative purposes as long as the focus is on the public API.
If there are trade-offs to be made in the implementation, or different implementation approaches that you might take,
this is a good time to discuss it. Once this step is far enough along that there is general agreement as to the API,
then it's time to focus on the implementation.

3. Submit a review of your proposed implementation. As noted in the
[New features / API additions](https://wiki.openjdk.java.net/display/OpenJFX/Code+Reviews#CodeReviews-NewFeaturesC.Newfeatures/APIadditions.)
section, we also need a [CSR](https://wiki.openjdk.java.net/display/csr/Main), which documents the API change and its approval.
The CSR can be reviewed in parallel. Changes in the API that arise during the review need to be reflected in the CSR, meaning
that the final review / approval of the CSR usually happens late in the review cycle

Note that a pull request is not the starting point, since that skips the first two important steps and jumps right into
"given this new feature, and an API definition that specifies it, please review my proposed implementation".
