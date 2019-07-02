let chunk = (arr: array('a), size: int): array(array('a)) => {
  let rec helper = (l: list('a), size: int): list(list('a)) => {
    Belt.(
      switch (l->List.take(size)) {
      | Some(slice) => [
          slice,
          ...helper(l->List.drop(size)->Option.getExn, size),
        ]
      | None => [l]
      }
    );
  };

  Belt.(
    arr
    ->List.fromArray
    ->helper(size)
    ->List.map(l => List.toArray(l))
    ->List.toArray
  );
};

let useKeyDown = (onKeyDown: Dom.keyboardEvent => unit) => {
  Webapi.Dom.(
    React.useEffect0(() => {
      Document.addKeyDownEventListener(onKeyDown, document);

      Some(() => Document.removeKeyDownEventListener(onKeyDown, document));
    })
  );
};