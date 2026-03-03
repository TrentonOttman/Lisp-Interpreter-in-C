(define map (f L) (cond
    (nil? L) ()
    't (cons (f (car L)) (map f (cdr L)))
))

(define filter (pred L) (cond
    (nil? L) ()
    (pred (car L)) (cons (car L) (filter pred (cdr L)))
    't (filter pred (cdr L))
))

(define reduce (f acc L) (cond
    (nil? L) acc
    't (reduce f (f acc (car L)) (cdr L))
))

(define length (L) (cond
    (nil? L) 0
    't (+ 1 (length (cdr L)))
))

(define nth (L n) (cond
    (nil? L) ()
    (= n 0) (car L)
    't (nth (cdr L) (- n 1))
))

(define drop (L n) (cond
    (nil? L) ()
    (= n 0) L
    't (drop (cdr L) (- n 1))
))

(define max (a b) (cond
    (> a b) a
    't b
))

(define min (a b) (cond
    (< a b) a
    't b
))

(define even? (a) (= (% a 2) 0))

(define odd? (a) (not (even? a)))

(define list3 (a b c)
    (cons a (cons b (cons c ())))
)

(define append (L M)(cond
    (nil? L) M
    't (cons (car L) (append (cdr L) M))
))