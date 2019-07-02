[@react.component]
let make = (~subjectId, ~reactionGroup) =>
  switch (reactionGroup##content) {
  | `THUMBS_UP =>
    <UI_Reaction_ThumbUp
      subjectId
      numberOfThumbUps=reactionGroup##users##totalCount
      viewerHasReacted=reactionGroup##viewerHasReacted
    />
  | `THUMBS_DOWN =>
    <UI_Reaction_ThumbDown
      subjectId
      numberOfThumbDowns=reactionGroup##users##totalCount
      viewerHasReacted=reactionGroup##viewerHasReacted
    />
  | _ => React.null
  };